TODO: Look at all of the TODOs here and see if they still are needed even after changes lol 

TODO: Move imgui folder (and any other externals into externals folder)
TODO: Move Catch2 into externals folder. Also figure out submodules. Why 
do we have Catch2 and json as submodules, but imgui is not? Does it 
matter? How do we make sure that if I pull on one machine it automatically 
works and going to build doesn't result in an error because it can't find 
the submodule?


NEXT STEP: Create a physics systems 
   - Step 1: We probably need to refactor the way we do views of a registry, so move that from Renderer and into its own thing, 
   then, work on defining iterators for that view (and perhaps our registry as well) so that we can iterate through it in a way 
   that makes sense
   - This tracks all the particles in its system, and is responsible for things like force repulsion. This system is responsible 
   for calculating the various forces each element may feel, as well as how this is done. For example: In a system of particles it can 
   partition the system into a grid and only check a certain amount of surrounding cells for inter-particle repulsion forces so as to 
   not result in an O(n^2) check time every single frame. Or it could take a different approach, but it is responsible for calculating 
   and updating the forces 
   - Will also track system stats like energy and stuff of that nature 
   - Question: Lets say we have 10 objects that react to gravity and other forces. If we want these to rebound off the floor, how do 
   we add the floor to our system so that it is not affected by gravity, and just stays there?
   ANSWER: body types: Static (the floor) which does not get updated due to forces, Dynamic (the particles) move and react to forces, 
   Kinematic (later): objects controlled by a specific function, script, or other method which means they do move, but not according to 
   the usual forces
   - Question: What do we want this API to look like? Add entity IDs in and their body type? 
   - Question: For the registry/component pools, how can we ensure that the registry has the correct component (i.e. position/velo)? How 
   do we ensure that if an entity has that component, it does not get removed accidentally? If an entity in the system doesn't have an entry 
   what should we do? (Partial answer: I think we, for now, log when this happens)

NEXT NEXT STEP: Implement Solvers
   - Decide how to partition solvers. Questions to ask:
      - Should we have on single base Solver class which Solvers (and sub-classes of solvers) inherit? 
      OR should we have a class for solvers which do Brownian motion vs other types of physical systems 
ANSWER: 
   - Something like this: 
      class Solver; class GenericEulerMaruyama : Solver; class BrownianMotionEulerMaruyama : GenericEulerMaruyama;
      where BMEM would specify would specifiy the functions it needed, and give the ability to define what constants 
      the user could control?

      - If we keep all solvers together should we add an enum for selecting the solver type?
ANSWER: 
   I think an enum with values like SDE, ODE, etc?

      - How do we define what a user needs to provide for these? Will they need the same inputs? 
      - Do we let the user provide a function for the 'stepper functions' or only give them control over 
      variables within those said functions? 
      - Maybe like a static assert and then the RenderDevice (or whatever needs it) can tell you what 
      types it needs to contain. How to do the stuff with validating GraphicsComperands though? TBD

GRAPHICS TODO(s) LEFT:
  - Add a way to translate our uniform blocks to std140. Right now it doesn't matter as 
  we only have one block with a single mat4 but in the future
  - Error handling in general (i.e. parsing/loading of .obj or .fmat files, buffer creation, etc...)
  - Also just errors in general
  - Better way to track buffer handles. What happens if we delete a buffer?

ECS TODO(s): 
  - Wrote a simple ForwardIterator for SortedComponentPool. Do we want one for SortedRegistry?
  - Update current tests (all broken) for the registry/pool, and add some for the iterator, and 
  registry view
  - Update anything that iterates through SortedComponentPool to use the iterator 
  - Separators? Do we use them actually?
  - see std::tie page on cppreference, but they can use std::tie in a bool operator() overload 
  function for a struct which could be useful for modeling our compare/comperands in sorting


TODO: There is a way to use our own math class with ImGui
TODO: Do some stuff in Camera class with LOCKED vs DIRECTIONAL
TODO: Add testing capabilities for different classes

TODO: Implementing GLShaderProgram. Make it so that it can take a GLShader shared pointer as an argument (probably a virtual func) to attach. 
Add to destructor of GLShader to delete it to free memory. In GLShaderProgram, detach GLShader in destructor from each shared pointers. Do linking stuff

TODO: Projection matrix is ever so slightly off of what glm::perspective returns. Specifically in the first element and 6th i.e. second row second column
TODO: Think about some cool ways to overload vector/matrix funcs to allow for some more useful and different stuff
TODO: Look into how to handle (for now just in vec/mat stuff but in the future more heavy calculations) things like loss of precision, when should a small number just become 0, etc..
TODO: Implement a Fig enum for datatypes and have imgui handle translating so that layers don't need to know about ImGuiDataType
TODO: Implement Tree and refactor stuff to use that
TODO: Implement actual structures for Fig::Events (and maybe begin actually handling them in app), 
TODO: ImGuiWidgets folder (i.e. reusable widgets like color picker)
TODO: Decouple application from imgui (see things like linux window)


   /* 
   std::cout << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10);
   dvec3 vec1 {3929.31038, 9804.72841, 5063.86373};
   dvec3 vec2 {6935.28254, 7699.60976, 3030.65272};
   dvec3 vec3 {6051.64732, 3844.55456, 6890.15868};
   dvec3 vec4 {1741.83093, 7555.56280, 3415.21740};

   dvec3 vec5 {9059.37915, 2793.78649, 3076.63488};
   dvec3 vec6 {3307.81531, 1392.47996, 6361.62214};
   dvec3 vec7 {5227.21071, 1810.54073, 7185.46577};
   // 6488.09236, 1666.50859,  588.44051

   Matrix<double, 4, 3> mat1 { vec1, vec2, vec3, vec4 };
   Matrix<double, 3, 3> mat2 { vec5, vec6, vec7 };
   std::cout << "mat1: " << mat1;
   std::cout << "mat2: " << mat2;

   auto res = mat1 * mat2;
   std::cout << "mat1 * mat2 = " << res;
   */
   // THIS EXAMPLE AS A FLOAT LOSES PRECISION (Notice they are currently double)


Stuff for EulerMaruyama solver ( might be changed up later)
I believe the solver works correctly. See the MSD (Mean Squared Distance) check below. Might be good idea to look into other
ways to check the validity of the solver. Now just need to create some plots of the particle, and check dimensions past 1. Also,
remove the added bits from the solver which were used for investigating things. Also, look into refactoring solver since we no longer subtract to find the distance and allat.

The expected MSD is: 4.4e-05
The average MSD after 10000 iterations for 1 time steps is: 4.36658e-05
The average MSD after 10000 iterations for 5 time steps is: 4.31166e-05
The average MSD after 10000 iterations for 10 time steps is: 4.3788e-05
The average MSD after 10000 iterations for 50 time steps is: 4.40551e-05
The average MSD after 10000 iterations for 100 time steps is: 4.45078e-05
The average MSD after 10000 iterations for 500 time steps is: 4.43106e-05
The average MSD after 10000 iterations for 1000 time steps is: 4.373e-05
The average MSD after 10000 iterations for 5000 time steps is: 4.3849e-05
The average MSD after 10000 iterations for 10000 time steps is: 4.39099e-05
The average MSD after 10000 iterations for 50000 time steps is: 4.46298e-05

DEV DEPENDENCIES: 
- If working on linux: 
   - Need 'libx11-dev' for x11 headers, BUT should just use 'xorg-dev' if doing OpenGL as well as 
   'xorg-dev' provides glx bindings
- For testing, need to clone Catch2 locally (at least until I figure out the submodule)
   - Catch2 is ignored for now in .gitignore

HELP STUFF:
 - If pointer stops moving when a key is down, there is likely a 'disable touchpad while typing' setting on 
