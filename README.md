# GZ Plugins, A reference for custom plugins


## Software Configuration

- GZ SIM VERSION 7.9.0 (Gazebo Garden)

## Notes For setup After Building:
 - The building process is done using **cmake** to get the make instructions and **make** for building, all commands given within the build folder; standard stuff
 - Please note I got sidetracked and did this so the codebase is rough around the edges and does not follow the ABI arch of the standard gz sim plugins, will do this later on if time permits
 - TBA: adding Bias and Dynamic Bias to the sensor readings and also to extend this for magnetometer sensor plugin of the gz sim
 - After building copy paste these cmds given below
 
 - For the GZ SYSTEM PLUGIN:
  ```bash
 export GZ_SIM_SYSTEM_PLUGIN_PATH=$GZ_SIM_SYSTEM_PLUGIN_PATH:<path_to_your_build_folder>
   ```

 - For the GZ GUI PLUGIN:
 ```bash
 export GZ_GUI_PLUGIN_PATH=$GZ_GUI_PLUGIN_PATH:<path_to_your_build_folder>
 ```


## Demo Video

 - **Vid-1** [Without any Sensor Noise or Bias]
 
 <video src="media/imu_ideal.mp4" controls width="100%"></video>



 - **Vid-2** [With Sensor Noise and Bias using Gaussian Probability Distribution]


 - **Vid-3** [Working of the custom gui plugin for dynamically changing the sensor model params]


## Notes:
- The GZ SIM API docs may change with versions used so the reference link to that document provided in the references section is for the Garden Version or GZ SIM VER(7)


## References
- [basics_on_gz_sim_plugins](https://gazebosim.org/api/sim/8/createsystemplugins.html)
- [some bare bones examples](https://gazebosim.org/api/sim/8/migrationplugins.html)
- [gz_sim_plugin can be used as templates to write custom plugins](https://github.com/gazebosim/gz-sim/blob/gz-sim7/src/systems/joint_position_controller/JointPositionController.cc#L484)
- [GZ SIM API documentation link](https://gazebosim.org/api/sim/7/classgz_1_1sim_1_1EntityComponentManager.html#a6d3cc1a13259adb16e330f6e8df42c66)
