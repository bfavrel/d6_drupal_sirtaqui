<?php
// $Id$

/**
 * @file
 * It Generates imagefield content 
 *
 * @author Benoit Favrel
 */

function get_imagefield_widget_shape($name, $label, $group = false) {
	
	return array (
    	'label' => $label,
    	'field_name' => $name,
    	'type' => 'filefield',
    	'widget_type' => 'imagefield_widget',
    	'change' => 'Modifier les informations de base',
    	'weight' => '0',
    	'file_extensions' => 'png gif jpg jpeg',
    	'progress_indicator' => 'bar',
    	'file_path' => '',
    	'max_filesize_per_file' => '',
    	'max_filesize_per_node' => '',
    	'max_resolution' => 0,
    	'min_resolution' => 0,
    	'custom_alt' => 0,
    	'alt' => '',
    	'custom_title' => 0,
    	'title_type' => 'textfield',
    	'title' => '',
    	'use_default_image' => 0,
    	'default_image_upload' => '',
    	'default_image' => NULL,
    	'description' => '',
		'group' => $group,
    	'required' => 0,
    	'multiple' => '1',
    	'list_field' => '0',
    	'list_default' => 1,
    	'description_field' => '0',
    	'op' => 'Enregistrer les paramètres du champ',
    	'module' => 'filefield',
    	'widget_module' => 'imagefield',
    	'columns' => array (
      		'fid' => array (
        		'type' => 'int',
        		'not null' => false,
        		'views' => true,
      		),
      		'list' => array (
        		'type' => 'int',
        		'size' => 'tiny',
        		'not null' => false,
        		'views' => true,
      		),
      		'data' => array (
        		'type' => 'text',
        		'serialize' => true,
        		'views' => true,
      		),
    	),
    	'display_settings' => array (
      		'label' => array (
        		'format' => 'hidden',
        		'exclude' => 0,
      		),
	      	'teaser' => array (
	        	'format' => 'image_plain',
	        	'exclude' => 0,
	      	),
	      	'full' => array (
	        	'format' => 'image_plain',
	        	'exclude' => 0,
	      	),
      		4 => array (
        		'format' => 'image_plain',
        		'exclude' => 0,
      		),
      		'token' => array (
        		'format' => 'image_plain',
        		'exclude' => 0,
      		),
    	),
  	);
}