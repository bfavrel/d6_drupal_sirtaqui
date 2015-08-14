<?php
// $Id$

/**
 * @file
 * It Generates date field content 
 *
 * @author Benoit Favrel
 */

function get_date_text_shape($name, $label, $group = false) {
	
	return array (
    	'label' => $label,
    	'field_name' => $name,
    	'type' => 'datetime',
    	'widget_type' => 'date_text',
    	'change' => 'Modifier les informations de base',
    	'weight' => '0',
		'group' => $group,
    	'default_value' => 'blank',
    	'default_value2' => 'same',
    	'default_value_code' => '',
    	'default_value_code2' => '',
    	'input_format' => 'd/m/Y - H:i:s',
    	'input_format_custom' => '',
    	'advanced' => array (
      		'label_position' => 'above',
      		'text_parts' => array (
        		'year' => 0,
        		'month' => 0,
        		'day' => 0,
        		'hour' => 0,
        		'minute' => 0,
        		'second' => 0,
      		),
    	),
    	'increment' => 1,
    	'year_range' => '-3:+3',
    	'label_position' => 'hidden',
    	'text_parts' => array (
    	),
    	'description' => '',
    	'required' => 0,
    	'multiple' => '0',
    	'repeat' => 0,
    	'todate' => 'optional',
    	'granularity' => array (
      		'year' => 'year',
      		'month' => 'month',
      		'day' => 'day',
    	),
    	'default_format' => 'medium',
    	'tz_handling' => 'none',
    	'timezone_db' => 'UTC',
    	'op' => 'Enregistrer les paramètres du champ',
    	'module' => 'date',
    	'widget_module' => 'date',
    	'columns' => array (
      		'value' => array (
        		'type' => 'varchar',
        		'length' => 20,
        		'not null' => false,
        		'sortable' => true,
        		'views' => true,
      		),
    	),
    	'display_settings' => array (
      		'label' => array (
        		'format' => 'above',
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