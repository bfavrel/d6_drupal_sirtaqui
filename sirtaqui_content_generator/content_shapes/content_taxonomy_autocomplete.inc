<?php
// $Id$

/**
 * @file
 * It Generates content taxonomy with auto complete content 
 *
 * @author Benoit Favrel
 */

function get_content_taxonomy_autocomplete_shape($name, $label, $vid, $group = false) {
	
	return array (
    	'label' => $label,
    	'field_name' => $name,
    	'type' => 'content_taxonomy',
    	'widget_type' => 'content_taxonomy_autocomplete',
    	'change' => 'Modifier les informations de base',
    	'weight' => '0',
    	'new_terms' => 'insert',
    	'extra_parent' => '0',
   		'maxlength' => '255',
    	'description' => '',
		'group' => $group,
    	'default_value' => array (
    	),
    	'default_value_php' => '',
    	'default_value_widget' => array (
      		$name => array (
        		'value' => '',
      		),
    	),
    	'required' => 0,
    	'multiple' => '1',
    	'save_term_node' => 1,
    	'vid' => strval($vid),
    	'parent' => '',
    	'parent_php_code' => '',
    	'depth' => '',
    	'op' => 'Enregistrer les paramètres du champ',
    	'module' => 'content_taxonomy',
    	'widget_module' => 'content_taxonomy_autocomplete',
    	'columns' => array (
      		'value' => array (
        		'type' => 'int',
        		'not null' => false,
        		'sortable' => false,
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