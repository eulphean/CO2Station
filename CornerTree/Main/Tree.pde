class Tree {
   ArrayList<Branch> branches; 
   float yOff = 0.1;
   boolean isNoise = false;
   
   // Constructor
   Tree() {
      print("Tree constructor: " + "\n");
      // Select a random height and startX position for this tree.
      //int treeHeight = (int) random(40, 75); 
      // For a field.
      //int startX = (int) random (3, width - 3);
      
      // Array of branches for this tree. 
      branches = new ArrayList();
      // A branch has a starting location, a starting "velocity", and a starting "timer" 
      color c = color(255,255,255);
      
      Branch b = new Branch(new PVector(width, height),new PVector(-2.0,-2.0), 150, c);
      b.isRoot = true;
      
      // Add to branches
      branches.add(b);
   }
   
   void draw() {
      if (isNoise) {
         yOff += 0.05;
      }
      
      // Animate and draw branches. 
      for (int i = 0; i < branches.size(); i++) {
        // Get the branch, update and draw it. 
        Branch b =  branches.get(i);
        if (b.isAnimating) {
          b.animate();
        }
        
        // Always render, every branch.
        b.render();
      }
      
      // [Perlin disabled currently. Come back and fix it.  Constrain the lengths and 
      // check flailer example from openFrameworks to see the 
      // applyPerlin();
   }
   
   void clear() {
      branches.clear(); 
   }

   void split() {
      // Go through the tree and check if there is a non-animating branch that we 
      // can split. If we can, then create a random number of branches at random
      // angles from there.
      for (int i = 0; i < branches.size(); i++) {
         Branch b = branches.get(i);
         // Branch shouldn't be animating.
         // Branch shouldn't be a child.
         if (!b.isAnimating && b.isChild) {
            // No more a child. So, we don't split it the next time. 
            b.isChild = false;
            
            // Random number of branches
            int n = (int) random(1, 5); 
            for (int j=0; j < n; j++) {
              color c = color(255, 255, 255);
              Branch newB = b.branch(random(-45, 45), c);
              branches.add(newB);
            }
         }
      }
      
      print("New branch size : " + branches.size() + "\n");
   }
   
   // Maybe this should be in branch.
   // Apply perline noise to branch angles.
   void applyPerlin() { 
      // Track non-animating count
      int nonAnimating = 0;
      for (int i=0; i < branches.size(); i++) {
        Branch b = branches.get(i);
        if (!b.isAnimating) {
          nonAnimating++;
        }
      }
      
      // Is something animating? No, apply perlin.
      if (nonAnimating == branches.size()) {
        isNoise = true;
        // We can apply perlin noise. 
        for (int i=0; i < branches.size(); i++) {
          if (!branches.get(i).isRoot) {
            Branch b = branches.get(i);
            float oldTheta = b.vel.heading2D();
            float newTheta = map(noise(i, yOff+i), 0, 1, oldTheta - PI/4, oldTheta + PI/4);
            // Calculate length
            float mag = (PVector.sub(b.end, b.start)).mag();
            // calculate new end vertex 
            b.end.x = mag*cos(newTheta); b.end.y = mag*sin(newTheta);
          }
        }
      } else {
         isNoise = false; 
      }
    }
}
