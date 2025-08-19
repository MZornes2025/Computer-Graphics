# CS 330 Computational Graphics and Visualization - 3D Scene Project

## Project Description
This project demonstrates the creation of a 3D scene using OpenGL and C++. The scene depicts a simple dining area, featuring a wooden table, stools, a ceramic cup, and a hanging chandelier within a textured room. The implementation focuses on applying core computer graphics principles, including object transformation, texture mapping, lighting, and interactive camera navigation.

## Repository Contents
*   `3dScene/` - Contains the Visual Studio project solution and source code files.
    *   `shadermanager.cpp` - The main rendering and scene management class.
    *   `(Other necessary .h and .cpp files)`
*   `Executable/` - Contains a built executable for running the 3D scene.
*   `DesignDecisions.pdf` - A document detailing the design process, object creation, and implementation strategies.
*   `Screenshots/` - Images of the final rendered 3D scene.

## Portfolio Reflection

### 1. How do I approach designing software?
My approach to software design, particularly in a graphical context, is highly iterative and structured. It begins with a clear objective—in this case, replicating a 2D image in 3D. The process then moves to decomposition, breaking down complex objects (like a cup or a table) into their fundamental geometric primitives (spheres, cylinders, tori, boxes). This modular mindset is crucial for managing complexity.

**What new design skills has your work on the project helped you to craft?**
This project honed my skills in **spatial systems design** and **resource management**. I learned to think not just in terms of code structure, but also in 3D coordinate space, understanding how objects relate to one another and to the virtual camera. Designing the material and lighting systems taught me how to create abstract data structures (like the `OBJECT_MATERIAL` struct) to manage complex visual properties efficiently.

**What design process did you follow for your project work?**
I followed a milestone-driven process:
1.  **Planning & Decomposition:** Selected a source image and identified the basic shapes needed for each object.
2.  **Foundation:** Built the core application structure and implemented the first transformed objects.
3.  **Interactivity:** Added camera controls using keyboard and mouse input.
4.  **Enhancement:** Applied textures and materials to define surface properties.
5.  **Atmosphere:** Implemented a multi-light Phong shading model to create depth and realism.
6.  **Integration & Polish:** Brought all elements together, adjusted placements, and refined the final output.

**How could tactics from your design approach be applied in future work?**
The tactic of breaking a large, complex problem (a full 3D scene) into smaller, manageable milestones is universally applicable. The practice of designing data structures around the project's needs—such as a system for tagging and retrieving textures and materials—is a powerful tool for creating organized, scalable, and maintainable code in any domain, from game engines to data-intensive applications.

### 2. How do I approach developing programs?
I approach development as a cycle of coding, testing, and refining. I start by implementing core functionality to establish a "vertical slice," then expand horizontally by adding features incrementally. Constant testing is integral to ensure each new feature works as intended and doesn't break existing functionality.

**What new development strategies did you use while working on your 3D scene?**
A key strategy was **abstraction through functions**. I created reusable functions like `SetTransformations()` and `SetShaderMaterial()` to avoid code duplication. This made the main rendering loop (`RenderScene()`) clean, readable, and easy to modify. Another strategy was **proactive asset management**, evident in the `CreateGLTexture()` and `FindTextureID()` functions, which handled loading and retrieving textures in a structured way.

**How did iteration factor into your development?**
Iteration was the core of development. Each milestone built upon the last. For example, the camera controls from Milestone Three were tested and then used to check the lighting from all angles in Milestone Five. Objects were placed, then moved, then scaled repeatedly to achieve the desired layout. Material properties like shininess and specular strength were adjusted dozens of times to get the right visual feel for wood, ceramic, and fabric.

**How has your approach to developing code evolved throughout the milestones?**
Initially, my approach was to just get something working. By the end, it evolved into writing code with **future use and modification in mind**. I began to see the application as a system of interconnected components rather than a single, monolithic script. This shift in perspective—from writing code to designing a system—is the most significant evolution in my development approach, leading to a much more robust and complete final project.

### 3. How can computer science help me in reaching my goals?
Computer science provides the foundational logic and problem-solving toolkit necessary to create solutions across countless domains. It teaches how to model real-world problems in a computational context and develop efficient, effective systems to address them.

**How do computational graphics and visualizations give you new knowledge and skills that can be applied in your future educational pathway?**
This project provided a concrete understanding of the mathematical underpinnings of 3D graphics—linear algebra, transformation matrices, and vector calculus—which are essential for advanced courses in computer vision, simulation, VR/AR, and GPU programming. It moves these concepts from pure theory to practical application, solidifying my understanding and providing a strong foundation for future academic exploration in these specialized fields.

**How do computational graphics and visualizations give you new knowledge and skills that can be applied in your future professional pathway?**
The skills developed here are directly applicable to numerous professional roles. Beyond the obvious paths in game development and visual effects, they are crucial in:
*   **Architectural Visualization:** Creating interactive 3D models of buildings.
*   **Product Design:** Prototyping and presenting products in a realistic 3D space.
*   **Data Visualization:** Representing complex data in an intuitive, 3D, interactive format.
*   **Simulation & Training:** Building virtual environments for training in fields like medicine, aviation, or manufacturing.
This project demonstrated the complete pipeline from concept to interactive product, a valuable experience for any software development role that involves creating user-facing graphical applications.
