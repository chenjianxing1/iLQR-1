## iLQR

Implementation of iLQR (Iterative Linear Quadratic Regulator) algorithm for trajectory optimization.

See `notes.md` for comments on algorithm, implementation, etc. 

### Usage

* Install eigen or download into `include` (https://eigen.tuxfamily.org/index.php?title=Main_Page#Download)
* `mkdir build; cd build`
* `cmake ..` 
* `make`
* Define a dynamics and cost model based on Model (see double_integrator example)

### Papers

* Tassa, Yuval, Nicolas Mansard, and Emo Todorov. "Control-limited differential dynamic programming." Robotics and Automation (ICRA), 2014 IEEE International Conference on. IEEE, 2014.
* Li, Weiwei, and Emanuel Todorov. "Iterative linear quadratic regulator design for nonlinear biological movement systems." ICINCO (1). 2004.
