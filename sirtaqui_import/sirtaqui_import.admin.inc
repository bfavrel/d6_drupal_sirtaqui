<?php
// $Id$

/**
 * @file
 * Creation sirtaqui admin page
 */

/***
 * LIST THE SIRTAQUI CONTENTS
 */

/**
 * Form Builder. List sirtaqui content
 *
 * @ingroup forms
 * @see sirtaqui_overview_sirtaqui_contents_submit()
 * @see theme_sirtaqui_overview_sirtaqui_contents()
 */
function sirtaqui_overview_sirtaqui_contents($form_state) {

	global $_sirtaqui_import_update_frequency;

	$languages = array();
	foreach (language_list() as $language)
		$languages[$language->language] = $language->name;

	$sirtaqui_content = sirtaqui_get_all_sirtaqui_contents();

	$content_type = node_get_types();

	// Purge in case content type has been removed
	foreach ($sirtaqui_content as $id => $content) {
		if (!isset($content_type['sirtaqui_'.$content->content_type]))
		{
			unset($sirtaqui_content[$id]);
			//delete from database
			sirtaqui_del_sirtaqui_content($content->id);
		}
	}

	$form = array('#tree' => TRUE);
	$sirtaqui = array();
	if (count($sirtaqui_content) > 0) {
		foreach ($sirtaqui_content as $content) {
			$sirtaqui[$content->content_type] = $content->content_name;
			//$form[$content->id]['#sirtaqui_content'] = (array) $content;
			$form[$content->id]['update_frequency'] = array('#value' => check_plain($_sirtaqui_import_update_frequency[$content->enable]));
			$form[$content->id]['name'] = array('#value' => check_plain($content->content_name));
			$form[$content->id]['language'] = array('#value' => $content->language);
			$form[$content->id]['module_id'] = array('#value' => $content->module_id);
			$form[$content->id]['objettour_code'] = array('#value' => $content->objettour_code);
			$form[$content->id]['edit'] = array('#value' => l(t('edit'), "admin/content/sirtaqui/edit/$content->id"));
			$form[$content->id]['edit_filters'] = array('#value' => l(t('edit filters'), "admin/content/sirtaqui/edit_filters/$content->id"));
			$form[$content->id]['delete'] = array('#value' => l(t('delete'), "admin/content/sirtaqui/delete/$content->id"));
			$form[$content->id]['import'] = array('#value' => l(t('update'), "admin/content/sirtaqui/update/$content->id"));
			$form[$content->id]['force_update'] = array('#value' => l(t('reset'), "admin/content/sirtaqui/update/$content->id/1"));
			$form[$content->id]['sirtaqui_xml'] = array(
				'#value' => l(
						t('see XML'),
						variable_get('sirtaqui_feed_url', 'http://cdt24.tourinsoft.com') . "/getDetailBordereau?idModule=" . $content->module_id . "&OBJETTOUR_CODE=" . $content->objettour_code,
						array('attributes' => array('target' => '_blank'))));
            $form[$content->id]['edit_params'] = array(
                '#value' => l(t("Parameters"), 'admin/content/sirtaqui/params/' . $content->id, array('query' => drupal_get_destination())),
            );
		}
	}

	$form['new_content_update_frequency'] = array(
		'#type' => 'select',
		'#title' => t('Update frequency'),
		'#required' => TRUE,
		'#options' => $_sirtaqui_import_update_frequency,
	);
	$form['new_content_type'] = array(
		'#type' => 'select',
		'#title' => t('Content Name'),
		'#required' => TRUE,
		'#options' => $sirtaqui,
		//'#size' => 15,
	);
	$form['new_content_language'] = array(
		'#type' => 'select',
		'#title' => t('Language'),
		'#required' => TRUE,
		'#options' => $languages,
		//'#size' => 15,
	);
	$form['new_content_module_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Module ID'),
		'#required' => TRUE,
		'#size' => 32,
	);
	$form['new_content_objettour_code'] = array(
		'#type' => 'textfield',
		'#title' => t('OBJETTOUR_CODE'),
		'#required' => TRUE,
		'#size' => 10,
	);

	$form['submit'] = array('#type' => 'submit', '#value' => t('Add'));

	return $form;
}

/**
 * Submit handler for sirtaqui content overview. Add a new sirtaqui content
 *
 * @see sirtaqui_overview_sirtaqui_contents()
 * @see theme_sirtaqui_overview_sirtaqui_contents()
 */
function sirtaqui_overview_sirtaqui_contents_submit($form, &$form_state) {

	$sirtaqui_content = sirtaqui_get_sirtaqui_contents();

	//krumo($form_state);

	// On crée le modèle dans la base
	$sirtaqui_content = array(
		'id' => '',
		'enable' => $form_state['values']['new_content_update_frequency'],
		'content_name' => $sirtaqui_content[$form_state['values']['new_content_type']]->content_name,
		'content_type' => $form_state['values']['new_content_type'],
		'language' => $form_state['values']['new_content_language'],
		'module_id' => $form_state['values']['new_content_module_id'],
		'objettour_code' => $form_state['values']['new_content_objettour_code'],
		'title_field' => $sirtaqui_content[$form_state['values']['new_content_type']]->title_field,
        'params' => serialize(array(
            'actions' => array(
                'not_in_sirtaqui' => 'unpublish',
                'unpublished_node' => 'publish',
            ),
        )),
	);

	if (!sirtaqui_add_sirtaqui_content($sirtaqui_content)) {
		drupal_set_message(t('Error while saving sirtaqui content %content', array('%content' => $sirtaqui_content['content_name'])), 'error');
	}
	else {
		drupal_set_message(t('Sirtaqui content %content has been created with language %language', array('%content' => $sirtaqui_content['content_name'], '%language' => $sirtaqui_content['language'])), 'status');
	}


	drupal_goto('admin/content/sirtaqui');
}

function theme_sirtaqui_overview_sirtaqui_contents($form) {

	$rows = array();

	foreach (element_children($form) as $key) {
		if (isset($form[$key]['name'])) {
			$theme = &$form[$key];

			$row = array();

			$row[] = drupal_render($theme['update_frequency']);
			$row[] = drupal_render($theme['name']);
			$row[] = drupal_render($theme['language']);
			$row[] = drupal_render($theme['module_id']);
			$row[] = drupal_render($theme['objettour_code']);
			$row[] = drupal_render($theme['edit']) . '<br>' .
						drupal_render($theme['edit_filters']) . '<br>' .
						drupal_render($theme['delete']) . '<br>' .
						drupal_render($theme['import']) . '<br>' .
						drupal_render($theme['force_update']) . '<br>' .
						drupal_render($theme['sirtaqui_xml']). '<br />' .
                        drupal_render($theme['edit_params']);
			//$row[] = drupal_render($theme['edit_filters']);
			//$row[] = drupal_render($theme['delete']);
			//$row[] = drupal_render($theme['import']);
			//$row[] = drupal_render($theme['force_update']);

			$rows[] = array('data' => $row, 'class' => '');
		}
	}
	if (empty($rows)) {
		$rows[] = array(array('data' => t('No themes available.'), 'colspan' => '6'));
	}

	// Add the new theme form
	$row = array();
	$row[] = drupal_render($form['new_content_update_frequency']);
	$row[] = drupal_render($form['new_content_type']);
	$row[] = drupal_render($form['new_content_language']);
	$row[] = drupal_render($form['new_content_module_id']);
	$row[] = drupal_render($form['new_content_objettour_code']);
	$row[] = drupal_render($form['submit']);

	$rows[] = array('data' => $row, 'class' => 'new_sirtaqui_content');

	$header = array(t('Frequency'), t('Content'), t('Language'), t('Module ID'), t('Bordereau'), t('Actions'));
	//$header[] = array('data' => t('Actions'), 'colspan' => '5');

	return theme('table', $header, $rows, array('id' => 'sirtaqui_contents')) . drupal_render($form);
}

/***
 * EDIT THE SIRTAQUI CONTENT
 */

/**
 * Form Builder. Edit sirtaqui content
 *
 * @ingroup forms
 * @see sirtaqui_edit_sirtaqui_content_submit()
 */
function sirtaqui_edit_sirtaqui_content($form_state, $id) {

	global $_sirtaqui_import_update_frequency;

	$content = sirtaqui_content_load($id);

	$sirtaqui = array();
	foreach (sirtaqui_content_load_all() as $sirtaqui_content)
		$sirtaqui[$sirtaqui_content->content_type] = $sirtaqui_content->content_name;

	$languages = array();
	foreach (language_list() as $language)
		$languages[$language->language] = $language->name;

	$sirtaqui_contents = sirtaqui_get_all_sirtaqui_contents();

	$form = array();
	$sirtaqui_occ = array();
	if (count($sirtaqui_contents) > 0) {
		foreach ($sirtaqui_contents as $content_occ) {
			if ($content_occ->id != $content->id) {
				//$form[$content->id]['#sirtaqui_content'] = (array) $content;
				$form[$content_occ->id]['update_frequency'] = array('#value' => check_plain($_sirtaqui_import_update_frequency[$content_occ->enable]));
				$form[$content_occ->id]['name'] = array('#value' => check_plain($content_occ->content_name));
				$form[$content_occ->id]['language'] = array('#value' => $content_occ->language);
				$form[$content_occ->id]['module_id'] = array('#value' => $content_occ->module_id);
				$form[$content_occ->id]['objettour_code'] = array('#value' => $content_occ->objettour_code);
				$form[$content_occ->id]['edit'] = array('#value' => l(t('edit'), "admin/content/sirtaqui/edit/$content_occ->id"));
				//$form[$content_occ->id]['delete'] = array('#value' => l(t('delete'), "admin/content/sirtaqui/delete/$content_occ->id"));
				//$form[$content_occ->id]['import'] = array('#value' => l(t('import'), "content/sirtaqui/import/$content_occ->id"));
			}
		}
	}

	$form['new_content_id'] = array(
		'#type' => 'hidden',
		'#value' => $content->id,
	);
	$form['new_content_update_frequency'] = array(
		'#type' => 'select',
		'#title' => t('Update frequency'),
		'#required' => TRUE,
		'#default_value' => $content->enable,
		'#options' => $_sirtaqui_import_update_frequency,
	);
	$form['new_content_type'] = array(
		'#type' => 'select',
		'#title' => t('Content Name'),
		'#required' => TRUE,
		'#options' => $sirtaqui,
		'#default_value' => $content->content_type,
		//'#size' => 15,
	);
	$form['new_content_language'] = array(
		'#type' => 'select',
		'#title' => t('Content language'),
		'#required' => TRUE,
		'#options' => $languages,
		'#default_value' => $content->language,
		//'#size' => 15,
	);
	$form['new_content_module_id'] = array(
		'#type' => 'textfield',
		'#title' => t('Module ID'),
		'#required' => TRUE,
		'#default_value' => $content->module_id,
		'#size' => 32,
	);
	$form['new_content_objettour_code'] = array(
		'#type' => 'textfield',
		'#title' => t('OBJETTOUR_CODE'),
		'#required' => TRUE,
		'#default_value' => $content->objettour_code,
		'#size' => 10,
	);

	$form['submit'] = array('#type' => 'submit', '#value' => t('Save'));

	return $form;
}

function sirtaqui_edit_sirtaqui_content_submit($form, &$form_state) {

	$content = sirtaqui_get_sirtaqui_contents();

	// On crée le modèle dans la base
	$sirtaqui_content = array(
		'id' => $form_state['values']['new_content_id'],
		'enable' => $form_state['values']['new_content_update_frequency'],
		'content_name' => $content[$form_state['values']['new_content_type']]->content_name,
		'content_type' => $form_state['values']['new_content_type'],
		'language' => $form_state['values']['new_content_language'],
		'module_id' => $form_state['values']['new_content_module_id'],
		'objettour_code' => $form_state['values']['new_content_objettour_code'],
		'title_field' => $content[$form_state['values']['new_content_type']]->title_field,
	);

	if (!sirtaqui_add_sirtaqui_content($sirtaqui_content)) {
		drupal_set_message(t('Error while saving sirtaqui content %content', array('%content' => $sirtaqui_content['content_name'])), 'error');
	}
	else {
		drupal_set_message(t('Sirtaqui content %content has been updated with language %language', array('%content' => $sirtaqui_content['content_name'], '%language' => $sirtaqui_content['language'])), 'status');
	}


	drupal_goto('admin/content/sirtaqui');
}

function theme_sirtaqui_edit_sirtaqui_content($form) {

	$rows = array();

	foreach (element_children($form) as $key) {
		if (isset($form[$key]['name'])) {
			$theme = &$form[$key];

			$row = array();

			$row[] = drupal_render($theme['update_frequency']);
			$row[] = drupal_render($theme['name']);
			$row[] = drupal_render($theme['language']);
			$row[] = drupal_render($theme['module_id']);
			$row[] = drupal_render($theme['objettour_code']);
			$row[] = drupal_render($theme['edit']);

			$rows[] = array('data' => $row, 'class' => '');
		}
	}
	if (empty($rows)) {
		$rows[] = array(array('data' => t('No themes available.'), 'colspan' => '6'));
	}

	// Add the new theme form
	$row = array();
	$row[] = drupal_render($form['new_content_update_frequency']);
	$row[] = drupal_render($form['new_content_type']);
	$row[] = drupal_render($form['new_content_language']);
	$row[] = drupal_render($form['new_content_module_id']);
	$row[] = drupal_render($form['new_content_objettour_code']);
	$row[] = drupal_render($form['submit']);

	$rows[] = array('data' => $row, 'class' => 'new_sirtaqui_content');

	$header = array(t('Update Frequency'), t('Content'), t('Language'), t('Module ID'), t('OBJETTOUR_CODE'), t('Actions'));
	//$header[] = array('data' => t('Actions'), 'colspan' => '3');

	return theme('table', $header, $rows, array('id' => 'sirtaqui_contents')) . drupal_render($form);
}

/***
 * EDIT CONTENT FILTERS
 */

/**
 * Form Builder. Edit sirtaqui content filters
 *
 * @ingroup forms
 * @see sirtaqui_edit_sirtaqui_content_filters_submit()
 * @see theme_sirtaqui_edit_sirtaqui_content_filters()
 */
function sirtaqui_edit_sirtaqui_content_filters($form_state, $id) {

	$content = sirtaqui_content_load($id);

	$content->import_filters = unserialize($content->import_filters);

	drupal_set_title(t('Edit import filters for ').$content->content_name);

	$form = array('#tree' => true);
	$form['id'] = array(
		'#type' => 'value',
		'#value' => $id,
	);
	if (!empty($content->import_filters)) {
		foreach ($content->import_filters as $field_name => $field_value) {
			$form['filters'][$field_name]['field'] = array(
				'#type' => 'textfield',
				'#title' => '',
				'#required' => true,
				'#default_value' => $field_name,
			);
			$form['filters'][$field_name]['value'] = array(
				'#type' => 'textfield',
				'#title' => t(''),
				'#default_value' => $field_value,
				'#required' => TRUE,
				'#size' => 40
			);
			$form['filters'][$field_name]['delete'] = array(
				'#type' => 'button',
				'#value' => t('Delete'),
				'#ahah' => array(
					'path' => 'admin/content/sirtaqui/content_listing/edit/js/delete_import_filter/'.$field_name,
					'wrapper' => $field_name,
					'effect' => 'fade',
					'method' => 'replace',
				),
			);
		}
	}

	$form['filters']['new_field']['field'] = array(
		'#type' => 'textfield',
	);
	$form['filters']['new_field']['value'] = array(
		'#type' => 'textfield',
		'#title' => '',
		'#size' => 40
	);

	$form['submit'] = array('#type' => 'submit', '#value' => t('Save filters'));

	return $form;
}

/**
 * Form Submit. Edit sirtaqui content filters
 *
 * @ingroup forms
 * @see sirtaqui_edit_sirtaqui_content_filters()
 * @see theme_sirtaqui_edit_sirtaqui_content_filters()
 */
function sirtaqui_edit_sirtaqui_content_filters_submit($form, &$form_state) {

	$sirtaqui_content = sirtaqui_content_load($form_state['values']['id']);

	$fields = array();
	foreach ($form_state['values']['filters'] as $key => $value) {
		if ($value['value'] != '')
			$fields[$value['field']] = $value['value'];
	}
	// On crée le modèle dans la base
	$sirtaqui_content = array(
		'id' => $form_state['values']['id'],
		'content_name' => $sirtaqui_content->content_name,
		'content_type' => $sirtaqui_content->content_type,
		'language' => $sirtaqui_content->language,
		'module_id' => $sirtaqui_content->module_id,
		'objettour_code' => $sirtaqui_content->objettour_code,
		'title_field' => $sirtaqui_content->title_field,
		'import_filters' => serialize($fields),
        'params' => $sirtaqui_content->params,
	);

	if (!sirtaqui_add_sirtaqui_content($sirtaqui_content)) {
		drupal_set_message(t('Error while saving sirtaqui content '), 'error');
		$form_state['redirect'] = "admin/content/sirtaqui/list";
		return;
	}

	return;
}

/**
 * Form Theme. Edit sirtaqui content filters
 *
 * @ingroup forms
 * @see sirtaqui_edit_sirtaqui_content_filters()
 * @see sirtaqui_edit_sirtaqui_content_filters_submit()
 */
function theme_sirtaqui_edit_sirtaqui_content_filters($form) {

	$rows = array();

	foreach (element_children($form['filters']) as $key) {
		if (isset($form['filters'][$key]['field']))
		{
			$theme = &$form['filters'][$key];

			$row = array();

			$row[] = drupal_render($theme['field']);
			$row[] = drupal_render($theme['value']);
			if (isset($theme['delete']))
				$row[] = drupal_render($theme['delete']);

			$rows[] = array('data' => $row,'class' => ' ', 'id' => $key);
		}
	}
	if (empty($rows)) {
		$rows[] = array(array('data' => t('No criterias available.'), 'colspan' => '3'));
	}

	$header = array(t("Field (XML name)"), t('Value'), t('Actions'));

	return theme('table', $header, $rows, array('id' => 'sirtaqui_content_import_filters')) . drupal_render($form);
}

function sirtaqui_delete_sirtaqui_content_filter($field_name) {

	// Récupération de l'ID unique du formulaire
	$form_build_id = $_POST['form_build_id'];

	// On fabrique un faux form_state
 	$form_state = array('submitted' => FALSE);

 	// Récupération du formulaire à partir du cache
	$form = form_get_cache($form_build_id, $form_state);

	// On enlève le critère demandé
	unset($form['filters'][$field_name]);

	// Sauvegarde du formulaire dans le cache
	form_set_cache($form_build_id, $form, $form_state);

	// Reconstruction du formulaire
	$form = form_builder($_POST['form_id'], $form, $form_state);

	drupal_json(array('data' => '', 'status' => TRUE));
	exit();
}

/***
 * DELETE THE SIRTAQUI CONTENT
 */

/**
 * Form builder for the sirtaqui content delete confirmation form.
 *
 * @ingroup forms
 * @see sirtaqui_content_themes_confirm_delete_submit()
 */
function sirtaqui_content_confirm_delete($form_state, $id) {

	$sirtaqui_content = sirtaqui_content_load($id);

	if (is_null($sirtaqui_content)) {
		drupal_set_message(t('Bad argument given'), 'error');
		drupal_goto('admin/content/sirtaqui');
	}

	$form['id'] = array('#type' => 'value', '#value' => $sirtaqui_content->id);
	$form['label'] = array('#type' => 'value', '#value' => $sirtaqui_content->content_name);

	return confirm_form($form,
						t('Are you sure you want to delete the content %title?', array('%title' => $sirtaqui_content->content_name)),
						'admin/content/sirtaqui',
						t('This action cannot be undone.'),
						t('Delete'),
						t('Cancel'));
}

/**
 * Submit handler to delete a sirtaqui content after confirmation.
 *
 * @see sirtaqui_content_confirm_delete()
 */
function sirtaqui_content_confirm_delete_submit($form, &$form_state) {

	sirtaqui_del_sirtaqui_content($form_state['values']['id']);

	drupal_set_message(t('Deleted sirtaqui content %label.', array('%label' => $form_state['values']['label'])));

	watchdog('sirtaqui_import', 'Deleted sirtaqui content %label.', array('%label' => $form_state['values']['label']), WATCHDOG_NOTICE);

	$form_state['redirect'] = 'admin/content/sirtaqui';

	return;
}

/*
function template_preprocess_sirtaqui_import_list(&$variables) {

	$modules = array();

	//$results = db_query('SELECT widget_settings, type_name FROM {content_node_field_instance} WHERE field_name = "field_idModule"');
	$results = db_query('SELECT * FROM {sirtaqui_content}');

	while ($result = db_fetch_object($results))
	{
		//$widget = unserialize($result->widget_settings);
		//$modules = array_merge($modules, array($result->type_name => $widget['default_value'][0]['value']));
		$modules = array_merge($modules, array($result->type_name => $result));
	}

	$variables['modules'] = $modules;
}
*/

function sirtaqui_import_secure_content($sirtaqui_content_id, $force_update = FALSE) {

	$hash = "83b863bb9b479595dfed63c01512f324";// Hash MD5 mot de passe SO
	sirtaqui_import_content($hash, $sirtaqui_content_id, FALSE, $force_update);
}

function sirtaqui_import_content($hash, $sirtaqui_content_id, $from_cron = FALSE, $force_update = FALSE) {

	if ($hash != "83b863bb9b479595dfed63c01512f324") {
		drupal_set_message(t('Access denied'));
		drupal_goto('/');
	}

    ini_set('max_execution_time', 12800);

	require_once 'sirtaqui_import.importer.inc';

    // TODO : cette fonction ne se trouve pas dans le bon module ! :
	$sirtaqui_content = sirtaqui_content_load($sirtaqui_content_id);
	$sirtaqui_content->import_filters = unserialize($sirtaqui_content->import_filters);

    $sirtaqui_content->params = unserialize($sirtaqui_content->params);
    $sirtaqui_content->params['actions']['unpublished_node'] = empty($sirtaqui_content->params['actions']['unpublished_node']) ? 'publish' : $sirtaqui_content->params['actions']['unpublished_node'];
    $sirtaqui_content->params['actions']['not_in_sirtaqui'] = empty($sirtaqui_content->params['actions']['not_in_sirtaqui']) ? 'unpublish' : $sirtaqui_content->params['actions']['not_in_sirtaqui'];

    watchdog('sirtaqui_import', "Update start : @language : @content<br />Type : @type", array(
            '@content' => $sirtaqui_content->content_name . " (id : " . $sirtaqui_content->id . ")",
            '@type' => 'sirtaqui_' . $sirtaqui_content->content_type,
            '@language' => $sirtaqui_content->language,
        ), WATCHDOG_INFO);

	// On charge le fichier contenant les filtres pour ce type de contenu (si il existe)
	require_once 'filters/sirtaqui_filters.inc';

	if (file_exists(drupal_get_path('module', 'sirtaqui_import') . '/filters/sirtaqui_'.$sirtaqui_content->content_type.'.inc')) {
		require_once 'filters/sirtaqui_' . $sirtaqui_content->content_type.'.inc';
	}

    $fnid_table = content_database_info(content_fields('fnid')); // s'il n'y a qu'un seul type, il n'y a pas de table distincte

	// On récupère les ID de tous les noeuds qui possèdent un fnid
    $req = "SELECT n.nid FROM {node} AS n
            LEFT JOIN {" . $fnid_table['table'] . "} AS fnid ON fnid.nid=n.nid
            WHERE n.type='sirtaqui_%s'
            AND n.language='%s'
            AND fnid.fnid_value IS NOT NULL";

	$results = db_query($req, $sirtaqui_content->content_type, $sirtaqui_content->language);

	$nids = array();
	while ($result = db_fetch_array($results)){
		$nids[$result['nid']] = TRUE;
	}

	//Récupère la liste de tous les champs du contenu
	$content_types = content_types('sirtaqui_'.$sirtaqui_content->content_type);

	// get the listing responses
	$attrs['idModule'] = $sirtaqui_content->module_id;
	$content_listing = sirtaqui_make_soap_request(variable_get('sirtaqui_feed_url', ''), variable_get('sirtaqui_get_listing_request', ''), $attrs);

	if ($content_listing != false) {

		$method = variable_get('sirtaqui_get_listing_request', '').'Result';

		//translate response to array
    	$xml = simplexml_load_string($content_listing->$method->any);

		//krumo($xml);
		$json = json_encode($xml);
		$listing_array = json_decode($json, TRUE);

		//krumo($content_types['fields']);
		if (!file_exists(file_directory_path().'/sirtaqui_files'))
			mkdir(file_directory_path().'/sirtaqui_files', 0775);

		//krumo($listing_array);
		if (isset($listing_array['Listing']['LISTING']['ID']))
		{
			$id = update_content_from_infos($sirtaqui_content, $listing_array['Listing']['LISTING'], $content_types['fields'], $force_update);
			unset($nids[$id]);
		}
		elseif (!empty($listing_array['Listing']['LISTING'])) {
			foreach ($listing_array['Listing']['LISTING'] as $infos) {

				//Création du contenu à partir des infos récupérées
				$id = update_content_from_infos($sirtaqui_content ,$infos, $content_types['fields'], $force_update);

				if (!is_null($id)) {
					// Mise à jours des infos de dispos si actif
					if (variable_get('sirtaqui_use_dispos', false) && module_exists('prestataires_dispos')) {
						require_once 'sirtaqui_import.dispo.inc';

						$contents_enable = variable_get('prestataires_dispos_content_types', array());

						$node = node_load($id, null, true);
						if ($contents_enable[$node->type]) {
							sirtaqui_import_update_dispos($node, $sirtaqui_content->module_id, $infos);
						}
					}
					unset($nids[$id]);
				}

			}
		}

		// traitement des contenus qui ne sont plus dans le flux
		if (!empty($nids)) {

            switch($sirtaqui_content->params['actions']['not_in_sirtaqui']) {
                case 'unpublish':
                    foreach ($nids as $nid => $val)
                    {
                        db_query("UPDATE {node} SET status=0 WHERE nid=%d", $nid);
                    }

                    break;

                case 'delete':
                    // le cron s'exécute en anonymous. on passe en admin avant les suppressions de nodes
                    global $user;
                    $original_user = $user;
                    $user = user_load(1);

                    foreach ($nids as $nid => $val) {
                        node_delete($nid);
                    }

                    // On restore le user original.
                    $user = $original_user;

                    break;
            }

        }
	}

    watchdog('sirtaqui_import', "Update over : @language : @content<br />Type : @type", array(
            '@content' => $sirtaqui_content->content_name . " (id : " . $sirtaqui_content->id . ")",
            '@type' => 'sirtaqui_' . $sirtaqui_content->content_type,
            '@language' => $sirtaqui_content->language,
        ), WATCHDOG_INFO);

	if (!$from_cron)
	{
		drupal_set_message(t('Content successfully created'));
		drupal_goto('/admin/content/sirtaqui/import_list');
	}
}

function sirtaqui_content_update($node) {

	// On test si le contenu est bien syndiqué
	if (empty($node->fnid[0]['value'])) {
		return;
	}

	// On récupère le module ID
	$sirtaqui_content = sirtaqui_content_load_by_type(preg_replace('/sirtaqui_/', '', $node->type), $node->language);

	if (!$sirtaqui_content) {
		return;
	}

	//Récupère la liste de tous les champs du contenu
	$content_types = content_types('sirtaqui_'.$sirtaqui_content->content_type);

	// On prépare les infos normalements présentes dans le listing
	$infos = array();
	$infos['OBJETTOUR_CODE'] = $sirtaqui_content->objettour_code;
	$infos['ID'] = $node->fnid[0]['value'];

	// On charge le fichier contenant les filtres pour ce type de contenu (si il existe)
	require_once 'filters/sirtaqui_filters.inc';
	if (file_exists(drupal_get_path('module', 'sirtaqui_import') . '/filters/sirtaqui_'.$sirtaqui_content->content_type.'.inc')) {
		require_once 'filters/sirtaqui_' . $sirtaqui_content->content_type.'.inc';
	}
	require_once 'sirtaqui_import.importer.inc';

	$id = update_content_from_infos($sirtaqui_content, $infos, $content_types['fields'], true);

	if (variable_get('sirtaqui_use_dispos', false) && module_exists('prestataires_dispos')) {
		require_once 'sirtaqui_import.dispo.inc';
		$contents_enable = variable_get('prestataires_dispos_content_types', array());

		if ($contents_enable[$node->type]) {
			sirtaqui_import_update_dispos($node, $sirtaqui_content->module_id, $infos);
		}
	}

	if ($id) {
		drupal_set_message('Content updated');
	}
	else {
		drupal_set_message('Error while updating', 'error');
	}

	drupal_goto($node->path);
}

/**
 *
 * Récupère les détails d'une fiche
 * @param string $module_id : l'identifiant du flux
 * @param array $infos : tableau contenant les champs du listing pour un contenu
 */
function _sirtaqui_get_content_details($module_id, $infos) {

	// On récupère le détail de la fiche
	$attrs['idModule'] = $module_id;
	$attrs['OBJETTOUR_CODE'] = $infos['OBJETTOUR_CODE'];
	$attrs['idOffre'] = $infos['ID'];
	$content_detail = sirtaqui_make_soap_request(variable_get('sirtaqui_feed_url', ''), variable_get('sirtaqui_get_detail_request', ''), $attrs);

	$method = variable_get('sirtaqui_get_detail_request', '').'Result';

	//translate response to array
	$xml = simplexml_load_string($content_detail->$method->any);
	//krumo($xml);
	$json = json_encode($xml);
	$detail_array = json_decode($json,TRUE);

	if (!$detail_array) { return array();}

	//krumo($detail_array['Listing']['DETAIL']);
	// Beaucoup d'erreur dans les champs détail déjà présent dans le listing
	foreach($infos as $field_name => $field)
		unset($detail_array['Listing']['DETAIL'][$field_name]);

	$infos = array_merge($infos, $detail_array['Listing']['DETAIL']);

	return $infos;
}

/**
 * Form Builder. Configure SIRTAQUI
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function sirtaqui_import_admin_settings(){

	$form = array();

	$form['sirtaqui_feed_url'] =  array(
		'#type'				=> 'textfield',
		'#title'			=> t('SIRTAqui feeds base url'),
		'#default_value'	=> variable_get('sirtaqui_feed_url', 'http://cdt24.tourinsoft.com/soft/RechercheDynamique/Syndication/controle/syndication2.asmx'),
		'#required'			=> TRUE,
		'#description'		=> t('Get the SIRTAqui webservice base url.')
	);
	$form['sirtaqui_field_list_request'] =  array(
		'#type'				=> 'textfield',
		'#title'			=> t('Feed request for content field list'),
		'#default_value'	=> variable_get('sirtaqui_field_list_request', 'getFieldsListing,getFieldsDetail'),
		'#required'			=> TRUE,
		'#description'		=> t('Get the webservice request to have the content field list. If more than one request, separate with a comma (,)')
	);
	$form['sirtaqui_get_listing_request'] =  array(
		'#type'				=> 'textfield',
		'#title'			=> t('Feed request for content listing'),
		'#default_value'	=> variable_get('sirtaqui_get_listing_request', 'getListing'),
		'#required'			=> TRUE,
		'#description'		=> t('Get the webservice request to have the content listing.')
	);
	$form['sirtaqui_get_detail_request'] =  array(
		'#type'				=> 'textfield',
		'#title'			=> t('Feed request for content details'),
		'#default_value'	=> variable_get('sirtaqui_get_detail_request', 'getDetail'),
		'#required'			=> TRUE,
		'#description'		=> t('Get the webservice request to have the content details.')
	);
	$form['sirtaqui_media_upload_url'] =  array(
		'#type'				=> 'textfield',
		'#title'			=> t('Url for media content'),
		'#default_value'	=> variable_get('sirtaqui_media_upload_url', 'http://cdt24.tourinsoft.com/upload/'),
		'#required'			=> TRUE,
		'#description'		=> t('Get the begin url of all media download.')
	);
	$form['sirtaqui_dispo_request'] =  array(
		'#type'				=> 'textfield',
		'#title'			=> t('Feed request for disponibilities'),
		'#default_value'	=> variable_get('sirtaqui_dispo_url', 'getDispo'),
		'#required'			=> TRUE,
		'#description'		=> t('Get the webservice request to have the disponibilities.')
	);
    $form['sirtaqui_separator'] =  array(
		'#type'				=> 'textfield',
		'#title'			=> t('Values separator'),
		'#default_value'	=> variable_get('sirtaqui_separator', '|'),
		'#required'			=> TRUE,
        '#size'             => 1,
		'#description'		=> t("Values separator used in XML fields ('|', '$', etc.)"),
	);
	$form['sirtaqui_use_dispos'] =  array(
		'#type'				=> 'checkbox',
		'#title'			=> t('Use Sirtaqui dispos'),
		'#default_value'	=> variable_get('sirtaqui_use_dispos', false),
		'#description'		=> t('Use disponibilities given by Tourinsoft database')
	);

    $form['logs'] = array(
        '#type' => 'fieldset',
        '#title' => t("Fiche logging"),
        '#description' => t("What must we log in watchdog about fiches updates ?"),
        'fiche_logs' => array(
            '#type' => 'checkboxes',
            '#title' => null,
            '#options' => array(
                'next' => t("Fiche next to be processed"),
                'filter' => t("Import filtering"),
                'abortion' => t("Abortion"),
                'up_to_date' => t("'Up to date' state"),
                'start' => t("Creation or update start"),
                'end' => t("Creation or update end"),
            ),
            '#default_value' => variable_get('fiche_logs', array()),
        ),
    );

	return system_settings_form($form);
}

function sirtaqui_import_params($form_state, $id) {
    $form = array();

    $form['#tree'] = true;

    $form['sirtaqui_content_id'] = array(
        '#type' => 'value',
        '#value' => $id,
    );

    $sirtaqui_content = db_fetch_object(db_query("SELECT * FROM {sirtaqui_content} WHERE id=%d", $id));
    $params = unserialize($sirtaqui_content->params);

    $form['actions'] = array(
        'not_in_sirtaqui' => array(
            '#type' => 'select',
            '#title' => t("Action to take for existing nodes missing in sirtaqui"),
            '#options' => array(
                'none' => t("None"),
                'unpublish' => t("Unpublish"),
                'delete' => t("Delete"),
            ),
            '#default_value' => !empty($params['actions']['not_in_sirtaqui']) ? $params['actions']['not_in_sirtaqui'] : 'unpublish',
        ),
        'unpublished_node' => array(
            '#type' => 'select',
            '#title' => t("Action to take for unpublished nodes existing in sirtaqui"),
            '#options' => array(
                'none' => t("None"),
                'publish' => t("Publish"),
            ),
            '#default_value' => !empty($params['actions']['unpublished_node']) ? $params['actions']['unpublished_node'] : 'publish',
        ),
    );

    $form['title_update_restriction'] = array(
        '#type' => 'checkbox',
        '#title' => t("Update title only if it is empty or in case of new node creation"),
        '#return_value' => true,
        '#default_value' => $params['title_update_restriction'],
    );

    $form['title_fields'] = array(
        '#type' => 'fieldset',
        '#title' => t("Title field settings override"),
        '#description' => t("Fields to use instead field set at content type creation.<br />Fields will be looked over from top to bottom, until a title is found.<br />From the moment where there is an override, default field won't be used anymore."),
        '#theme' => 'sirtaqui_import_params_title_fields',
        'field_default' => array(
            '#type' => 'markup',
            '#value' => t("Default field used") . " : &lt;" . $sirtaqui_content->title_field . "/&gt;",
            '#prefix' => "<div>",
            '#suffix' => "</div>",
        ),
    );

    $form['title_fields']['fields'] = array();
    $form['title_fields']['weights'] = array();

    foreach((array)$params['title_fields'] as $title_field => $field_infos) {
        unset($form['title_fields']['field_default']);

        $form['title_fields']['fields'][$title_field] = array(
            '#type' => 'markup',
            '#value' => "&lt;" . $title_field . "/&gt;",
        );

        $form['title_fields']['weights'][$title_field] = array(
            '#type' => 'weight',
            '#title' => t("Weight"),
            '#default_value' => $field_infos['weight'],
        );

        $form['title_fields']['delete_buttons'][$title_field] = array(
            '#type' => 'submit',
            '#value' => t("Delete"),
            '#op' => 'delete_field',
            '#field' => $title_field,
            '#name' => 'delete_field-' . $title_field,
        );
    }

    $form['title_fields']['add_title_field'] = array(
        'field' => array(
            '#type' => 'textfield',
            '#title' => t("Field (XML name)"),
            '#description' => t("Raw name, without tags"),
        ),
        'add' => array(
            '#type' => 'submit',
            '#op' => 'add_field',
            '#value' => t("Add field"),
            '#validate' => array('sirtaqui_import_params_add_title_field_validate'),
        ),
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t("Save"),
    );

    $form['back'] = array(
        '#value' => l(t("Back"), $_GET['destination']),
    );

    return $form;
}

function sirtaqui_import_params_add_title_field_validate($form, &$form_state) {
    if(empty($form_state['values']['title_fields']['add_title_field']['field'])) {
        form_set_error('title_fields][add_title_field][field', t("You have to provide a field name"));
    }
}

function sirtaqui_import_params_submit($form, &$form_state) {
    $form_state['rebuild'] = true;

    $id = $form_state['values']['sirtaqui_content_id'];

    $sirtaqui_content = db_fetch_object(db_query("SELECT * FROM {sirtaqui_content} WHERE id=%d", $id));
    $params = unserialize($sirtaqui_content->params);

    $params['actions'] = $form_state['values']['actions'];

    $params['title_update_restriction'] = $form_state['values']['title_update_restriction'];

    if($form_state['clicked_button']['#op'] == 'add_field') {
        $params['title_fields'][$form_state['values']['title_fields']['add_title_field']['field']] = array(
            'weight' => 50,
        );
    }

    if(!empty($params['title_fields'])) {
        $weights = array();

        foreach ($params['title_fields'] as $field => $infos) {
            $params['title_fields'][$field]['weight'] = $form_state['values']['title_fields']['weights'][$field];
            $weights[$field] = $params['title_fields'][$field]['weight'];
        }

        array_multisort($weights, SORT_ASC, $params['title_fields']);

        if($form_state['clicked_button']['#op'] == 'delete_field') {
            unset($params['title_fields'][$form_state['clicked_button']['#field']]);
        }
    }

    $sirtaqui_content->params = serialize($params);

    drupal_write_record('sirtaqui_content', $sirtaqui_content, 'id');

    drupal_set_message(t("Parameters have been saved"), 'status');
}

function theme_sirtaqui_import_params_title_fields($element) {
    $output = "";

    $output .= drupal_render($element['override']);

    $rows = array();

    foreach(element_children($element['fields']) as $field) {

        $element['weights'][$field]['#attributes']['class'] .= ' field-weight';

        $row = array(
            drupal_render($element['fields'][$field]),
            drupal_render($element['weights'][$field]),
            drupal_render($element['delete_buttons'][$field]),
        );

        $rows[] = array('data' => $row, 'class' => 'draggable');
    }

    if(!empty($rows)) {
        $headers = array(t("Field (XML name)"), t("Weight"), '');
        $output .= theme('table', $headers, $rows, array('id' => 'title_fields_list'));
        drupal_add_tabledrag('title_fields_list', 'order', 'sibling', 'field-weight', null, null, true);
    } else {
        $output .= "<strong>" . drupal_render($element['field_default']) . "</strong>";
    }

    $row = array(
        array(
            drupal_render($element['add_title_field']['field']),
            drupal_render($element['add_title_field']['add']),
        )
    );

    $output .= theme('table', null, $row);

    return $output;
}