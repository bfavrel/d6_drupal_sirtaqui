<?php
// $Id$

/**
 * @file 
 * Creation sirtaqui content page
 */

/**
 * 
 * Get the module ID to get fields list
 * @param unknown_type $form_state
 */
function sirtaqui_content_generator_get_feeds_id($form_state) {
	
	if (variable_get('sirtaqui_feed_url', '') == '')
	{
		drupal_set_message(t('Thank you to configure the SIRTAqui module before use it.'), 'warning');
		drupal_goto('admin/settings/sirtaqui');
	}
	
	$form = array();
	
	$form['sirtaqui_feed_id'] = array(
		'#type'				=> 'textfield',
		'#title'			=> t('Module ID'),
		'#required'			=> TRUE,
		'#description'		=> t('')
	);
	
	$form['submit'] = array(
		'#value' => t('Rechercher'),
		'#type' => 'submit',
	);
	
	return $form;
}

/**
 * 
 * Execute at the form submition
 * @param array $form
 * @param array $form_state
 */
function sirtaqui_content_generator_get_feeds_id_submit($form, &$form_state) {
	
	$form_state['redirect'] = 'admin/content/types/sirtaqui/new_content/'.$form_state['values']['sirtaqui_feed_id'];

}

/**
 * 
 * Make the user choose fields to insert in content type
 * @param string $module_id
 */
function sirtaqui_content_generator_overview_field_list($form_state, $module_id = '') {
	
	if (empty($module_id))
		drupal_goto('admin/content/sirtaqui/new_content');
		
	$fields = sirtaqui_content_generator_get_fields($module_id);
	
	$languages = array();
	foreach (language_list() as $language)
		$languages[$language->language] = $language->name;
		
	// get the possible field content type
	$types = sirtaqui_content_generator_get_field_content_type();
	
	$form = array();
	$form['#tree'] = TRUE;
	
	// Creation of the content type
	
	$form['content_properties'] = array(
		'#type' => 'fieldset',
		'#title' => t('Content detail'),
	);
	$form['content_properties']['content_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Content name'),
		'#required' => TRUE
	);
	$form['content_properties']['content_type'] = array(
		'#type' => 'textfield',
		'#title' => t('Content type'),
		'#required' => TRUE,
		'#description' => t('Content name in the drupal system.')
	);
	$form['content_properties']['content_description'] = array(
		'#type' => 'textarea',
		'#title' => t('Content description'),
	);
	$form['content_properties']['module_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Module ID'),
		'#value' => $module_id,
		'#required' => TRUE,
		'#attributes' => array('readonly' => 'readonly'),
	);
	$form['content_properties']['title_field'] = array(
		'#type' => 'select',
		'#title' => t('Champs utilisé pour le titre'),
		'#required' => TRUE,
		'#options' => $fields,
	);
	$form['content_properties']['content_language'] = array(
		'#type' => 'select',
		'#title' => t('Champs utilisé pour le titre'),
		'#required' => TRUE,
		'#default_value' => language_default()->language,
		'#options' => $languages,
	);
	
	$form['content_fields'] = array(
		'#type' => 'fieldset',
		'#title' => t('Content fields'),
	);
	if (count($fields) > 0)
	{
		// On récupère les vocabulaires existants
		$vocabularies = array("" => '', "0" => t("-- Create new --"));
		foreach (taxonomy_get_vocabularies() as $vid => $v)
		{
			$vocabularies[$vid] = $v->name;
		}
		
		foreach($fields as $field)
		{
			$field_type = normalize_field_name($field);
			$field_occ = content_fields(normalize_field_name('field_'.$field));
			if (!is_null($field_occ))
			{
				$f_type = $field_occ['widget']['type'];
				if ($f_type == 'text_textfield' && $field_occ['multiple']) $f_type = 'multiple_text_textfield';
				
				$form['content_fields'][$field_type]['type'] = array('#type' => 'select', '#options' => $types, '#default_value' => $f_type, '#attributes' => array('readonly' => 'readonly'));
			}
			else
			{
				$form['content_fields'][$field_type]['type'] = array(
					'#type' => 'select',
					'#options' => $types,
					/*'#ahah' => array(
						'event' => 'change',
						'path' => 'admin/content/types/sirtaqui/new_content/add_taxo/'.$field_type,
						'wrapper' => $field_type.'-vid-select',
						'effect' => 'fade',
      					'method' => 'replace',
					)*/
				);
			}
			
			$form['content_fields'][$field_type]['vid'] = array(
				'#type' => 'select',
				'#options' => $vocabularies,
				'#required' => FALSE
			);
			$form['content_fields'][$field_type]['name'] = array('#value' => check_plain($field));
			$form['content_fields'][$field_type]['label'] = array('#type' => 'textfield', '#required' => true, '#default_value' => check_plain($field));
			//$form['content_fields'][$field_type]['type'] = array('#type' => 'select', '#options' => $types);
			$form['content_fields'][$field_type]['enable'] = array('#type' => 'checkbox', '#default_value' => true);
			$form['content_fields'][$field_type]['weight'] = array('#type' => 'weight', '#delta' => 30, '#default_value' => 0);
		}
	}
	
	$form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
	$form['#redirect'] = "admin/content/types/sirtaqui";
	
	return $form;
}

function sirtaqui_content_generator_overview_field_list_submit($form, &$form_state) {
	
	ini_set('max_execution_time', 600);
	
	// On importe tous les modèles de champs
	include 'content_shapes/generic_content.inc';
	include 'content_shapes/fieldgroup.inc';
	foreach (sirtaqui_content_generator_get_field_content_type() as $type => $name)
		include 'content_shapes/'.$type.'.inc';
		
	// On récupère tous les vocabulaires de taxonomy
	$default_vocabularies = taxonomy_get_vocabularies();
		
	// On crée le modèle dans la base
	$sirtaqui_content = array(
		'id' => '',
		'content_name' => $form_state['values']['content_properties']['content_name'],
		'content_type' => $form_state['values']['content_properties']['content_type'],
		'language' => $form_state['values']['content_properties']['content_language'],
		'module_id' => $form_state['values']['content_properties']['module_id'],
		'title_field' => $form_state['values']['content_properties']['title_field'],
	);
	
	if (!sirtaqui_add_sirtaqui_content($sirtaqui_content))
	{
		drupal_set_message(t('Error while saving sirtaqui content %content', array('%content' => $sirtaqui_content['content_name'])), 'error');
		drupal_goto('admin/content/types/sirtaqui/new_content/'.$sirtaqui_content['module_id']);
	}
	
	$content = get_generic_content_shape(	$form_state['values']['content_properties']['content_type'],
											$form_state['values']['content_properties']['content_name'], 
											$form_state['values']['content_properties']['content_description']);
	
	//array_push($content['fields'], get_text_textfield_shape('field_objettour_code', 'OBJETTOUR_CODE'));
	array_push($content['fields'], get_text_textfield_shape('date_maj', 'DATEMAJ'));
	array_push($content['fields'], get_text_textfield_shape('fnid', 'Feeds node ID'));
	
	$existing_fields = array(); // On stocke la liste des noms des champs au cas où avec la normalisation 2 champs se retrouvent égaux
	$vocabularies = array(); // listes des identifiants des vocabulaires pour tous les champs de taxo
	foreach ($form_state['values']['content_fields'] as $field_name => $field)
	{
		if (!array_key_exists(normalize_field_name('field_'.$field_name), $existing_fields))
		{
			$existing_fields = array_merge($existing_fields, array(normalize_field_name('field_'.$field_name) => TRUE));

			// Si le contenu est de type "content_taxonomy_autocomplete" on crée un vocabulaire au nom du champs
			if ($field['enable'] && $field['type'] == 'content_taxonomy_autocomplete')
			{
				$vocabulary = array();
				
				if ($field['vid'] == '0' || $field['vid'] == '')
				{
					$vocabulary = array(
					    'name' => $field['label'],
					    'multiple' => 1,
					    'required' => 0,
					    'hierarchy' => 0,
					    'relations' => 1,
					    'tags' => 1,
					    'weight' => 0,
						'module' => 'taxonomy',
					    'nodes' => array('sirtaqui_'.$form_state['values']['content_properties']['content_type'] => 1),
					);
				}
				else
				{
					$nodes = array();
					foreach ($default_vocabularies[$field['vid']]->nodes as $node)
					{
						$nodes[$node] = 1;
					}
					$nodes['sirtaqui_'.$form_state['values']['content_properties']['content_type']] = 1;
					
					$vocabulary = array(
						'vid' => $field['vid'],
					    'name' => $default_vocabularies[$field['vid']]->name,
					    'multiple' => $default_vocabularies[$field['vid']]->multiple,
					    'required' => $default_vocabularies[$field['vid']]->required,
					    'hierarchy' => $default_vocabularies[$field['vid']]->hierarchy,
					    'relations' => $default_vocabularies[$field['vid']]->relations,
					    'tags' => $default_vocabularies[$field['vid']]->tags,
					    'weight' => $default_vocabularies[$field['vid']]->weight,
						'module' => $default_vocabularies[$field['vid']]->module,
					    'nodes' => $nodes,
					);
				}
				taxonomy_save_vocabulary($vocabulary);
				
				$vocabularies[$field_name] = $vocabulary['vid'];
				
				array_push($content['fields'], call_user_func('get_'.$field['type'].'_shape', normalize_field_name('field_'.$field_name), $field['label'], $vocabulary['vid']));
			}
			else
			{
				if ($field['enable'])
					array_push($content['fields'], call_user_func('get_'.$field['type'].'_shape', normalize_field_name('field_'.$field_name), $field['label']));
			}
		}
	}
	array_push($content['groups'], get_fieldgroup_shape('sirtaqui', 'SIRTAQUI'));
	foreach ($form_state['values']['content_fields'] as $field_name => $field)
	{
		if (!array_key_exists(normalize_field_name('field_f_'.$field_name), $existing_fields))
		{
			$existing_fields = array_merge($existing_fields, array(normalize_field_name('field_f_'.$field_name) => TRUE));
		
			// Si le contenu est de type "content_taxonomy_autocomplete" on crée un vocabulaire au nom du champs
			if ($field['enable'] && $field['type'] == 'content_taxonomy_autocomplete')
			{
				array_push($content['fields'], call_user_func('get_'.$field['type'].'_shape', normalize_field_name('field_f_'.$field_name), $field['label'], $vocabularies[$field_name], 'group_sirtaqui'));
			}
			else
			{
				if ($field['enable'])
					array_push($content['fields'], call_user_func('get_'.$field['type'].'_shape', normalize_field_name('field_f_'.$field_name), $field['label'], 'group_sirtaqui'));
			}
		}
	}
	
	$macro = "";
	foreach ($content as $form_type => $form)
		$macro .= "\$content['$form_type']  = ". var_export((array) $form, TRUE) .";\n";
	
	create_content_type($macro);

	drupal_flush_all_caches();
}

/**
 * Theme the field list overview
 * 
 * @ingroup themeable
 * @see sirtaqui_content_generator_overview_field_list($fields)
 * @param unknown_type $form
 */
function theme_sirtaqui_content_generator_overview_field_list($form) {
	
	$rows = array();
	
	foreach (element_children($form['content_fields']) as $key) {
		if (isset($form['content_fields'][$key]['name'])) {
			$field = &$form['content_fields'][$key];

			$row = array();
			$field['weight']['#attributes']['class'] = 'field-weight';
			$row[] = drupal_render($field['weight']);
			$row[] = drupal_render($field['enable']);
			$row[] = drupal_render($field['name']) . drupal_render($field['label']);
			$row[] = drupal_render($field['type']);
			if (isset($field['vid']))
				$row[] = drupal_render($field['vid']);
			$rows[] = array('data' => $row, 'class' => 'draggable');
		}
	}
	if (empty($rows)) {
		$rows[] = array(array('data' => t('No fields available.'), 'colspan' => '4'));
	}

	$header = array(t('Weight'), t('Enable'), t('Name'), t('Field Type'), t('Vocabulary'));
	if (isset($form['submit'])) {
		drupal_add_tabledrag('sirtaqui_content', 'order', 'sibling', 'field-weight');
	}
	
	$form['content_fields'] = array(
		'#type' => 'markup',
        '#value' => theme('table', $header, $rows, array('id' => 'sirtaqui_content')),
        '#weight' => '1',
    );
	
	//$form['content_fields']['#suffix'] = theme('table', $header, $rows, array('id' => 'sirtaqui'));
	
	return drupal_render($form) ;
}

function sirtaqui_content_generetor_add_taxo($field_type) {
	
	$submitted = $_POST['content_fields'];
	
	if ($submitted[$field_type]['type'] == 'content_taxonomy_autocomplete')
	{
		// On récupère les vocabulaires existants
		$vocabularies = array("" => t("-- Create new --"));
		foreach (taxonomy_get_vocabularies() as $vid => $v)
		{
			$vocabularies[$vid] = $v->name;
		}
		$new_field['content_fields'][$field_type]['vid'] = array(
			'#type' => 'select',
			'#options' => $vocabularies,
			'#name' => 'content_fields['.$field_type.'][vid]',
			'#id' => 'edit-content-field-'.preg_replace('/_/', '-', $field_type).'-vid',
			'#required' => FALSE
		);
	
		drupal_json(array('status' => TRUE, 'data' => drupal_render($new_field['content_fields'][$field_type]['vid'])));
		return;
	}
	
	drupal_json(array('status' => TRUE, 'data' => ''));
}

function sirtaqui_content_generator_get_fields($module_id) {

	$attrs = array();
	$result = null;
	$fields = array();
	
	$attrs['idModule'] = $module_id;
	
	$requests = preg_split('/,/', trim(variable_get('sirtaqui_field_list_request', '')));
	
	if (!empty($requests))
	{
		foreach ($requests as $request)
		{
			$result = sirtaqui_make_soap_request(variable_get('sirtaqui_feed_url', ''), $request, $attrs);

			$method = $request.'Result';
			
			$xml = simplexml_load_string($result->$method->any);
			
			if ($xml->count() > 0)
				foreach($xml->children()->children() as $item)
				{
					foreach($item as $name => $value)
					{
						if ($name == "LISTINGFIELD_NAME")
						{
							$fields = array_merge($fields, array((string) normalize_field_name($value, 300, FALSE, FALSE) => (string) $value));
							break;
						}
						elseif ($name == "DETAILFIELD_NAME")
						{
							$fields = array_merge($fields, array((string) normalize_field_name($value, 300, FALSE, FALSE) => (string) $value));
							break;
						}
					}
				}
		}
	}
	
	return $fields;
}

/**
 * 
 * @return the different content type possible for each fields
 */
function sirtaqui_content_generator_get_field_content_type() {
	
	$field_type = array();
	
	if (module_exists('text')) {
		$text = array(
			'multiple_text_textfield' => t('Multiple textfield'),
			'text_textfield' => t('Text field'),
			'text_textarea' => t('Text area'),
			//'text_bool' => t('Boolean')
		);
		$field_type = array_merge($field_type, $text);
	}
	if (module_exists('filefield')) {
		$filefield = array(
			'filefield_widget' => 'Fichier'
		);
		$field_type = array_merge($field_type, $filefield);
	}
	if (module_exists('imagefield')) {
		$imagefield = array(
			'imagefield_widget' => 'Image / Photo'
		);
		$field_type = array_merge($field_type, $imagefield);
	}
	if (module_exists('content_taxonomy_autocomplete')) {
		$taxo = array(
			'content_taxonomy_autocomplete' => 'Taxonomy auto-complétion'
		);
		$field_type = array_merge($field_type, $taxo);
	}
	/*
	if (module_exists('content_taxonomy_options')) {
		$taxo = array(
			'content_taxonomy_options' => 'Taxonomy case à cocher'
		);
		$field_type = array_merge($field_type, $taxo);
	}
	*/
	if (module_exists('date')) {
		$date = array(
			'date_text' => 'Date'
		);
		$field_type = array_merge($field_type, $date);
	}
	
	return $field_type;
}

/**
 * 
 * Fonction qui crée le type de contenu dans Drupal
 * @param string $content
 */
function create_content_type($content) {
   
    // Build form state
    $form_state = array(
      'values' => array(
           'type_name' => '<create>',
           'macro' => $content,
     ),
    );
    // content_copy is a module for importing & exporting CCK types
    drupal_execute("content_copy_import_form", $form_state);
    content_clear_type_cache();
}
