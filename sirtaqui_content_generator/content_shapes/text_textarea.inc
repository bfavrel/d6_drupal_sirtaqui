<?php
// $Id$

/**
 * @file
 * It Generates textarea content 
 *
 * @author Benoit Favrel
 */

function get_text_textarea_shape($name, $label, $group = false) {
	
	return array (
	    'label' => $label,
	    'field_name' => $name,
	    'type' => 'text',
	    'widget_type' => 'text_textarea',
	    'change' => 'Modifier les informations de base',
	    'weight' => '0',
	    'rows' => '5',
	    'size' => 60,
	    'description' => '',
		'group' => $group,
	    'default_value' => array (
      		0 => array (
        		'value' => '',
        		'_error_element' => 'default_value_widget]['.$name.'][0][value',
      		),
    	),
    	'default_value_php' => '',
    	'default_value_widget' => array (
      		$name => array (
        		0 => array (
          			'value' => '',
          			'_error_element' => 'default_value_widget]['.$name.'][0][value',
        		),
      		),
    	),
    	'required' => 0,
   		'multiple' => '0',
    	'text_processing' => '1',
    	'max_length' => '',
    	'allowed_values' => '',
    	'allowed_values_php' => '',
    	'op' => 'Enregistrer les paramètres du champ',
    	'module' => 'text',
    	'widget_module' => 'text',
    	'columns' => array (
      		'value' => array (
      			'type' => 'text',
        		'size' => 'big',
        		'not null' => false,
        		'sortable' => true,
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