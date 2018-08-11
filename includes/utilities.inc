<?php

/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

function collection_members_result_metadata_updater($objects, $metadata){
    if(empty($objects) || !is_array($objects) || count($objects) == 0){
        throw new Exception("Empty or invalid array of collection members!", NULL, NULL);
    }
    if(empty($metadata)){
        throw new Exception("Empty or invalid metadata!", NULL, NULL);
    }
    module_load_include('module', 'islandora', 'islandora');
    foreach ($objects as $object) {
        $pid = $object['object']['value'];
        $islandora_object = islandora_object_load($pid);
        if($islandora_object){
            object_metadata_updater($islandora_object, $metadata);
        }
    }
}

/**
 * Modify object metadata
 *
 *
 * @param FedoraObject $object
 *   Size in bytes to convert
 * @param associative array $metadata
 *   
 *
 * @return 
 *  
 */
function object_metadata_updater($object, $metadata){
    if(empty($metadata)){
        throw new Exception("Empty or invalid metadata!", NULL, NULL);
    }
    if(empty($object)){
        throw new Exception("Empty or invalid object!", NULL, NULL);
    }
    $metadata_entry = $metadata['entry'];
    $metadata_info_arr = explode(":", $metadata_entry);
    if(!isset($metadata_info_arr) || count($metadata_info_arr) != 2){
        throw new Exception("The metadata entry is invalid!", null, null);
    }
    $metadata_type = strtoupper($metadata_info_arr[0]);  
    $metadata_entry_name = $metadata_info_arr[1];

    if($metadata_type == null){
        throw new Exception("The metadata type is NOT defined!", "", "");
    }                
    module_load_include('module', 'islandora', 'islandora');
    $datastream = islandora_datastream_load($metadata_type, $object);
    if($datastream == NULL){
        throw new Exception("Cannot find the datastream for object pid=$pid with datasteam id=$metadata_type", null, null);
    }
    else{
        $metadata_val = $metadata['value'];
        $content = $datastream->content;
        try{
            $doc = new DOMDocument();
            $doc->loadXML($content);
            $data_fields = $doc->getElementsByTagName($metadata_entry_name);
            if($data_fields->length == 0){
                $parent_node_name = $metadata['parent_node_name'];                
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
        } catch (Exception $ex) {
            throw new Exception($ex->getMessage(), null, null);
        }
    }
}