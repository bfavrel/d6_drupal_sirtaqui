<?php
// $Id$

/**
 * @file
 * It Generates fieldgroup
 *
 * @author Benoit Favrel
 */

function get_fieldgroup_shape($name, $label, $description = '') {
	
	return array (
    	'label' => $label,
    	'group_type' => 'standard',
    	'settings' => array (
      		'form' => array (
        		'style' => 'fieldset',
        		'description' => '',
      		),
      		'display' => array (
        		'description' => $description,
        		'teaser' => array (
          			'format' => 'fieldset',
          			'exclude' => 0,
        		),
	        	'full' => array (
	          		'format' => 'fieldset',
	          		'exclude' => 0,
	        	),
	        	4 => array (
	          		'format' => 'fieldset',
	          		'exclude' => 0,
	        	),
	        	'token' => array (
	          		'format' => 'fieldset',
	          		'exclude' => 0,
	        	),
	        	'label' => 'hidden',
      		),
    	),
    	'weight' => '1',
    	'group_name' => 'group_'.$name,
  	);
  
}