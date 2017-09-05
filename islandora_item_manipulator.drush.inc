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
      'metadata_type' => array(
        'description' => 'metadata type',
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
                $metadata_type = drush_get_option('metadata_type');
                if($metadata_type == null){
                    throw new Exception("The metadata type is NOT defined!", "", "");
                }                
                $datastream = islandora_datastream_load($metadata_type, $object);
                if($datastream == NULL){
                    throw new Exception("Cannot find the datastream for object pid=$pid with datasteam id=$metadata_type", null, null);
                }
                else{
                    $content = $datastream->content;
                    try{
                        $doc = new DOMDocument();
                        $doc->loadXML($content);
                        $data_fields = $doc->getElementsByTagName("datafield");
                        for($i = 0; $i < $data_fields->length; $i++){
                            $data_field = $data_fields->item($i);
                            $tag = $data_field->getAttribute("tag");
                            if(!empty($tag) && $tag == "035"){
                                $subfields = $data_field->getElementsByTagName("subfield");
                                if(!empty($subfields) && $subfields->length > 0){
                                    $subfield = $subfields->item(0);
                                    $val = $subfield->nodeValue;
                                    if(!empty($val) && $val == "HST4491"){
                                        $subfield->nodeValue = "This is Tao's test.";
                                    }
                                }
                            }
                        }
                        $content = $doc->saveXML();
                        $datastream->setContentFromString($content);
                        $object->ingestDatastream($datastream);
                        print_r($doc);
                    } catch (Exception $ex) {

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
