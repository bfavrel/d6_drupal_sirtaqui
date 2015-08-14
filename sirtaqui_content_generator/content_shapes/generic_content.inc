<?php
// $Id$

/**
 * @file
 * It Generates basic content 
 *
 * @author Benoit Favrel
 */

function get_generic_content_shape($name, $label, $description) {
	
	return array(
		'type' => array(
			'name' => $label,
  			'type' => 'sirtaqui_'.$name,
  			'description' => $description,
  			'title_label' => 'Titre',
  			'body_label' => 'Corps',
  			'min_word_count' => '0',
  			'help' => '',
  			'node_options' => array (
    			'status' => true,
    			'promote' => true,
    			'sticky' => false,
    			'revision' => false,
  			),
  			'language_content_type' => '1',
  			'old_type' => 'sirtaqui_'.$name,
			'orig_type' => '',
			'module' => 'node',
			'custom' => '1',
			'modified' => '1',
			'locked' => '0',
			'comment' => '0',
			'comment_default_mode' => '4',
			'comment_default_order' => '1',
			'comment_default_per_page' => '50',
			'comment_controls' => '3',
			'comment_anonymous' => 0,
			'comment_subject_field' => '1',
			'comment_preview' => '1',
			'comment_form_location' => '0',
			'page_title' => array (
    			'show_field' => array (
      				'show_field' => true,
    			),
    			'pattern' => '',
  			),
		),
		'fields' => array(),
		'groups' => array(),
		'extra' => array (
			'title' => '-5',
			'body_field' => '0',
  			'revision_information' => '20',
  			'author' => '20',
  			'options' => '25',
  			'comment_settings' => '30',
  			'menu' => '-2',
  			'path' => '30',
		)
	);
}