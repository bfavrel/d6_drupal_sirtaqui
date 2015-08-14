<?php
// $Id$

/**
 * @file
 * It Generates textfield content 
 *
 * @author Benoit Favrel
 */

function get_text_textfield_shape($name, $label, $group = false, $default_value = '') {
	
	return array (
	    'label' => $label,
	    'field_name' => $name,
	    'type' => 'text',
	    'widget_type' => 'text_textfield',
	    'change' => 'Modifier les informations de base',
	    'weight' => '0',
	    'rows' => 5,
	    'size' => '60',
	    'description' => '',
		'group' => $group,
	    'default_value' => array (
	    	0 => array (
	    		'value' => $default_value,
	    		'_error_element' => 'default_value_widget]['.$name.'][0][value',
	    	),
	    ),
	    'default_value_php' => '',
	    'default_value_widget' => NULL,
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
		    'value' =>  array (
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
			    'format' => 'plain',
			    'exclude' => 0,
		    ),
		    'full' => array (
		    'format' => 'plain',
		    'exclude' => 0,
		    ),
	    	4 => array (
	        	'format' => 'plain',
	        	'exclude' => 0,
	      	),
	      	'token' => array (
	        	'format' => 'plain',
	        	'exclude' => 0,
	      	),
	    ),
	);
}