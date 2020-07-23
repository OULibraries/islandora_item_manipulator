# islandora_item_manipulator
Custom drush command to read, and delete Islandora items.

Examples: 

* `drush -u 1 iim --pid=islandora:324423DLAJL --operation=delete ` 
* `drush -u 1 iim --pid=islandora:324423DLAJL --operation=read`

This is used to expose some item level functionality to CyberCommons.  
Update of items has not yet been implemented, and none of our CyberCommons workflows rely on this functionality as of 2020-07-23. 
