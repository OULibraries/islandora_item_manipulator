# islandora_item_manipulator
Custom drush command to read, update, delete Islandora items.

Examples: 

drush -u 1 iim --pid=islandora:324423DLAJL --operation=delete <br>

drush -u 1 iim --pid=islandora:324423DLAJL --operation=read  <br>

drush -u 1 iim --pid=islandora:324423DLAJL --operation=update --option=metadata --entry=dc:rights --parent_node_name=dc --value=http://rightsstatements.org/vocab/NoC-US/1.0/ <br>

# change item metadata in a collection:
drush -u 1 iim --pid=islandora:bookCollection --operation=update --option=collection_objects_metadata --entry=dc:rights --parent_node_name=dc --value=http://rightsstatements.org/vocab/NoC-US/1.0/
