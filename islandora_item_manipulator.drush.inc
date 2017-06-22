<?php 
/**
 * @file
 */

/**
 * Implements hook_drush_command().
 */
function islandora_item_manipulator_drush_command() {
  $items = array();

  $items['islandora_item_manipulator'] = array(
    'callback' => 'drush_islandora_item_manipulator',
    'aliases' => array('iim'),
    'description' => 'manipulate Islandora items',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
    'options' => array(
      'pid' => array(
        'description' => 'Item pid',
        'required' => TRUE,
      ),
      'operation' => array(
        'description' => 'Operation type',
        'required' => TRUE,
      ),
    )
  );

  return $items;
}

/**
 *
 */
function drush_islandora_item_manipulator() {
  module_load_include('module', 'islandora', 'islandora');
  $pid = drush_get_option('pid');
  $operation = drush_get_option('operation');
  try{
    switch($operation) {
      case "read" :
	$object = islandora_object_load($pid);
	print_r($object);
	return $object;
	break;
      case "update" :
	drupal_set_message("Start to update the object ".$pid); 
	break;
      case "delete" :
	drupal_set_message("Start to delete the object ".$pid);
	drupal_static_reset('islandora_get_tuque_connection'); 
	$tuque = islandora_get_tuque_connection();
  	if ($tuque) {
    	  try {
	    return $tuque->repository->purgeObject($pid);
    	  }
    	  catch (Exception $e) {
      	    if ($e->getCode() == '404') {
              return FALSE;
      	    }
      	    else {
	      drupal_set_message("Cannot purge the object ".$pid);
              return NULL;
      	    }
    	  }
  	}
  	else {
    	  IslandoraTuque::getError();
 	}
	drupal_set_message("Completed deletion of object ".$pid);	
	break;
      default :
        drupal_set_message("The operation ".$operation." is NOT implemented!");
 	break;
    }
  }
  catch(Exception $e){
    echo 'Message: ' .$e->getMessage();
    drupal_set_message($e->getMessage());
  }
}
