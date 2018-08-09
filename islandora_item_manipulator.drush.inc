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
      'option' => array(
        'description' => 'Option for the update action',
        'required' => FALSE,
      ),
      'entry' => array(
        'description' => 'metadata entry',
        'required' => FALSE,
      ),
      'value' => array(
        'description' => 'metadata entry value',
        'required' => FALSE,
      ),
        'parent_node_name' => array(
        'description' => 'parent xml node of the metadata entry to be modified',
        'required' => FALSE,
      ),
    )
  );

  return $items;
}

/**
 *  Detailed function handling object
 */
function drush_islandora_item_manipulator() {
  module_load_include('module', 'islandora', 'islandora');
  $pid = null;
  $operation = null;
  try{
    $pid = drush_get_option('pid');
    if($pid == null){
      throw new Exception("The pid is NOT defined!", "", "");
    }
    $operation = drush_get_option('operation');
    if($operation == null){
      throw new Exception("The operation is NOT defined!", "", "");
    }
  }
  catch(Exception $e){
      echo 'Message: ' .$e->getMessage();
      drupal_set_message($e->getMessage());
      return null;
  }
  
  $object = islandora_object_load($pid);
    if($object == NULL){
        throw new Exception("Cannot load the object with pid=$pid", null, null);
    }
        
  try{
    switch($operation) {
      case "read" :
	print_r($object);
	return $object;
	break;
      case "update" :
	drupal_set_message("Start to update the object ".$pid); 
        $update_option = drush_get_option('option');
        if($update_option == null){
            throw new Exception("The update option parameter is NOT defined!", "", "");
        }
        switch($update_option){
            case "metadata": 
                /**
                 * Metadata types: MODS, DC, RELS-EXT, etc.
                 */
                $metadata_entry = drush_get_option('entry');
                $metadata_info_arr = explode(":", $metadata_entry);
                if(!isset($metadata_info_arr) || count($metadata_info_arr) != 2){
                    throw new Exception("The metadata entry is invalid!", null, null);
                }
                $metadata_type = strtoupper($metadata_info_arr[0]);  
                $metadata_entry_name = $metadata_info_arr[1];
                        
                if($metadata_type == null){
                    throw new Exception("The metadata type is NOT defined!", "", "");
                }                
                $datastream = islandora_datastream_load($metadata_type, $object);
                if($datastream == NULL){
                    throw new Exception("Cannot find the datastream for object pid=$pid with datasteam id=$metadata_type", null, null);
                }
                else{
                    $metadata_val = drush_get_option('value');
                    $content = $datastream->content;
                    try{
                        $doc = new DOMDocument();
                        $doc->loadXML($content);
                        $data_fields = $doc->getElementsByTagName($metadata_entry_name);
                        if($data_fields->length == 0){
                            $parent_node_name = drush_get_option('parent_node_name');
                            if(!isset($parent_node_name)){
                                throw new Exception("Cannot insert the new metadata node into datastream without parent node name", null, null);
                            }
                            $node_list = $doc->getElementsByTagName($parent_node_name);
                            if(!isset($node_list) || $node_list->length == 0){
                                throw new Exception("Cannot find parent node from metadata datastream with parent node name: ".$parent_node_name, null, null);
                            }
                            $parent = $node_list->item(0);
                            $element = $doc->createElement($metadata_entry, $metadata_val);                            
                            $parent->appendChild($element);
                        }
                        else{
                            for($i = 0; $i < $data_fields->length; $i++){
                                $data_field = $data_fields->item($i);
                                $data_field->nodeValue = $metadata_val;
                            }  
                        }                                              
                        $content = $doc->saveXML();
                        $datastream->setContentFromString($content);
                        $object->ingestDatastream($datastream);
                        print_r($doc);
                    } catch (Exception $ex) {
                        throw new Exception($ex->getMessage(), null, null);
                    }
                }
                break;
            default :
                drupal_set_message("The update operation for ".$update_option." has NOT been implemented!");
                break;
        }
	break;
      case "delete" :
	drupal_set_message("Start to delete the object ".$pid);
	drupal_static_reset('islandora_get_tuque_connection'); 
	$tuque = islandora_get_tuque_connection();
        $response = "";
  	if ($tuque) {
    	  try {
	    $response = $tuque->repository->purgeObject($pid);
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
          drupal_set_message("Completed deletion of object ".$pid." at ".$response);
  	}
  	else {
    	  IslandoraTuque::getError();
 	}		
	break;
      default :
        drupal_set_message("The operation ".$operation." has NOT been implemented!");
 	break;
    }
  }
  catch(Exception $e){
    echo 'Message: ' .$e->getMessage();
    drupal_set_message($e->getMessage());
    return null;
  }
}
