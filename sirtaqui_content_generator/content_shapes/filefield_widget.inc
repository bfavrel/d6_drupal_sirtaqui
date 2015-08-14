<?php
// $Id$

/**
 * @file
 * It Generates filefield content 
 *
 * @author Benoit Favrel
 */

function get_filefield_widget_shape($name, $label, $group = false) {
	
	return array (
    	'label' => $label,
    	'field_name' => $name,
    	'type' => 'filefield',
    	'widget_type' => 'filefield_widget',
    	'change' => 'Modifier les informations de base',
    	'weight' => '0',
    	'file_extensions' => 'txt pdf',
    	'progress_indicator' => 'bar',
    	'file_path' => '',
    	'max_filesize_per_file' => '',
    	'max_filesize_per_node' => '',
    	'description' => '',
		'group'=> $group,
    	'required' => 0,
    	'multiple' => '1',
    	'list_field' => '0',
    	'list_default' => 1,
    	'description_field' => '0',
    	'op' => 'Enregistrer les paramètres du champ',
    	'module' => 'filefield',
    	'widget_module' => 'filefield',
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
        		'format' => 'default',
        		'exclude' => 0,
      		),
      		'full' => array (
        		'format' => 'default',
        		'exclude' => 0,
      		),
      		4 => array (
        		'format' => 'default',
       			'exclude' => 0,
      		),
      		'token' => array (
        		'format' => 'default',
       			'exclude' => 0,
      		),
    	),
  );
}