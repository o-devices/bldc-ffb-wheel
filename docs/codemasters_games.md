##To make the steering wheel work in codemasters games you need:

* Go to: ``\steamapps\common\DiRT Rally 2.0\input\devices``
* Edit file ``device_defines.xml`` by pasting at the end 
``` 
<device id ="{588A045B-0000-0000-0000-504944564944}" name = "hover_wheel" priority = "102" type = "wheel" Official = "false" /> 
```
* Go to: ``\steamapps\common\DiRT Rally 2.0\input\actionmaps``
* Create file ``hover_wheel.xml`` and paste inside 
```
<?xml version="1.0" encoding="utf-8"?>
<action_map name="hover_wheel" device_name="hover_wheel" library="lib_direct_input">
  <axis_defaults>
    <axis name="di_x_axis">
    <action deadzone="0" name="driving.steer.left" />
    <action deadzone="0" name="driving.steer.right" />
    </axis>
  </axis_defaults>
  <group name="driving">
    <group name="steer">
      <action name="left">
       <axis name="di_x_axis" type="lower" />
      </action>
     <action name="right">
      <axis name="di_x_axis" type="upper" />
    </action>
  </group>
 </group>
</action_map>
```