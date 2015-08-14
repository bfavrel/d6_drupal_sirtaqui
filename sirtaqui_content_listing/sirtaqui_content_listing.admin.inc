<?php
// $Id$

/**
 * @file
 * Administrate content listing
 *
 * @author Benoit Favrel
 */

/**
 * Settings form
 */
function sirtaqui_content_listing_admin_settings() {

    $form = array();

    // Pour la compilation des valeurs en array pour variable_set();
    $form['#submit'][] = 'sirtaqui_content_listing_admin_settings_submit';
    $form['#tree'] = true;

    $form['scl_num_results_by_page'] = array(
        '#type' => 'textfield',
        '#title' => t('Results by page'),
        '#default_value' => variable_get('scl_num_results_by_page', 10),
        '#required' => true,
        '#size' => 4,
    );

    $form['scl_search_store_inputs'] = array(
        '#type' => 'checkbox',
        '#title' => t("Keep user search inputs stored permanently"),
        '#description' => t("If unchecked, user search will be cleared everytime a user leave a listing."),
        '#default_value' => variable_get('scl_search_store_inputs', true),
    );

    // TODO : à supprimer lors de la migration D7
    $form['scl_search_compat_mode'] = array(
        '#type' => 'checkbox',
        '#title' => t("Use compatibility with old version"),
        '#description' => t("For websites which already have content listings set when they go updated with the new version of module (featuring Forms!)<br />
            In the new version, forms and listing are created together. Whith compatibility mode set, some checks are performed when admin displays the config form, and if form doesn't exist in database it is created.<br />
            Disabling this options if not needed can save a couple of database queries."),
        '#default_value' => variable_get('scl_search_compat_mode', true),
    );

    $form['hook_scl_query_order'] = array(
        '#type' => 'fieldset',
        '#title' => "hook_scl_query() execution order",
        '#description' => t("This hook is called at the very end of the process, just before SQL compilation.<br />
            Order start at '1'. If set to '0', hook will be discarded."),
    );

    $hooks_scl_query = module_implements('scl_query');
    $hooks_scl_query_values = variable_get('hook_scl_query_order', array());

    foreach($hooks_scl_query as $hook) {

        $form['hook_scl_query_order'][$hook] = array(
            '#type' => 'textfield',
            '#title' => $hook,
            '#default_value' => isset($hooks_scl_query_values[$hook]) ? $hooks_scl_query_values[$hook] : 1,
            '#size' => 1,
            '#weight' => isset($hooks_scl_query_values[$hook]) ? $hooks_scl_query_values[$hook] : 1,
        );
    }

    $children = element_children($form['hook_scl_query_order']);

    if(empty($children)) {
        $form['hook_scl_query_order']['empty'] = array(
            '#type' => 'markup',
            '#value' => '- ' . t("No hook set") . ' -',
        );
    }

    $form['orderby'] = array(
        '#type' => 'fieldset',
        '#title' => t("ORDER BY"),
        '#description' => t("If hook sirtaqui_content_listing_scl_query() has been activated, it's possible to choose how to integer its clause against existing ORDER BY set by other modules."),
    );

    $form['orderby']['scl_query_orderby_mode'] = array(
        '#type' => 'radios',
        '#title' => t("hook sirtaqui_content_listing_scl_query() behavior"),
        '#description' => t("At this point, filters have been processed"),
        '#default_value' => variable_get('scl_query_orderby_mode', 1),
        '#options' => array(
            1 => t("Prepend"),
            2 => t("Append"),
            3 => t("Replace"),
        ),
        '#disabled' => $hooks_scl_query_values['sirtaqui_content_listing'] == 0,
    );

    $form['scl_debug_query'] = array(
        '#type' => 'checkbox',
        '#title' => t("Debug mode"),
        '#description' => t("Display SCL queries in Drupal messages zone (only for UID 1 and in front end)"),
        '#default_value' => variable_get('scl_debug_query', 0),
    );

    return system_settings_form($form);
}

/*
 * Submit function, to compile multiple variables in arrays
 */
function sirtaqui_content_listing_admin_settings_submit($form, &$form_state) {

    variable_set('hook_scl_query_order', $form_state['values']['hook_scl_query_order']);
    variable_set('scl_query_orderby_mode', $form_state['values']['orderby']['scl_query_orderby_mode']); // à cause de #tree, on doit traiter manuellement
}

/***
 * LIST THE SIRTAQUI CONTENT LISTINGS
 */

/**
 * Form Builder. List sirtaqui content listing
 *
 * @ingroup forms
 * @see sirtaqui_overview_sirtaqui_content_listings_submit()
 * @see theme_sirtaqui_overview_sirtaqui_content_listings()
 */
function sirtaqui_overview_sirtaqui_content_listings($form_state) {

	$listings = sirtaqui_get_sirtaqui_content_listings();

    $field_infos = content_fields('field_content_listing');
    $field_content_listing = content_database_info($field_infos);

    $languages = language_list();
    
	$form = array();
    
	if (count($listings) > 0)
		foreach ($listings as $listing)
		{
            $req = "SELECT n.nid, n.title, n.language FROM {node} AS n
                    RIGHT JOIN {" . $field_content_listing['table'] . "} AS ctl ON ctl.nid=n.nid
                    WHERE ctl." . $field_content_listing['columns']['value']['column'] . "=%d
                    ORDER BY n.language DESC, n.title ASC";

            $result = db_query($req, $listing->id);

            $pages = array();
            

            while($row = db_fetch_object($result)) {
                $pages[] = l($row->title . " (" . (empty($row->language) ? t('Language neutral') : t($languages[$row->language]->name)) . ")", drupal_get_path_alias('node/' . $row->nid));
            }

            $pages = implode("<br />", $pages);

			$form[$listing->id]['name'] = array('#value' => check_plain($listing->content_listing_name));
            $form[$listing->id]['pages'] = array('#type' => 'markup', '#value' => $pages);
			$form[$listing->id]['description'] = array('#value' => $listing->description);
			$form[$listing->id]['edit'] = array('#value' => l(t('edit'), "admin/content/sirtaqui/content_listing/edit/$listing->id"));
			$form[$listing->id]['delete'] = array('#value' => l(t('delete'), "admin/content/sirtaqui/content_listing/delete/$listing->id"));
		}

	return $form;
}

/**
 * Form Theme. List sirtaqui content listing
 *
 * @ingroup forms
 * @see sirtaqui_overview_sirtaqui_content_listings()
 * @see sirtaqui_overview_sirtaqui_content_listings_submit()
 */
function theme_sirtaqui_overview_sirtaqui_content_listings($form) {

	$rows = array();

	foreach (element_children($form) as $key) {
		if (isset($form[$key]['name']))
		{
			$theme = &$form[$key];

			$row = array();

			$row[] = drupal_render($theme['name']);
            $row[] = drupal_render($theme['pages']);
			$row[] = drupal_render($theme['description']);
			$row[] = drupal_render($theme['edit']);
			$row[] = drupal_render($theme['delete']);

			$rows[] = array('data' => $row, 'class' => '');
		}
	}
	if (empty($rows)) {
		$rows[] = array(array('data' => t('No listing available.'), 'colspan' => '5'));
	}

	$header = array(t('Listing'), t("Pages"), t('Description'));
	$header[] = array('data' => t('Actions'), 'colspan' => '2');

	return theme('table', $header, $rows, array('id' => 'sirtaqui_content_listings')) . drupal_render($form);
}

/***
 * CREATE A NEW SIRTAQUI CONTENT LISTING
 */

/**
 * Form Builder. Create sirtaqui content listing
 *
 * @ingroup forms
 * @see sirtaqui_create_sirtaqui_content_listing_submit()
 * @see theme_sirtaqui_create_sirtaqui_content_listing()
 */
function sirtaqui_create_sirtaqui_content_listing(&$form_state) {

	$content_type = node_get_types('names');

	if (count($content_type) > 0)
		foreach ($content_type as $type => $name) {
			if (!preg_match('/^sirtaqui_.*/', $type))
				unset($content_type[$type]);
		}

	$form = array();

	$form['content_listing_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Listing Name'),
		'#required' => TRUE,
	);
	$form['description'] = array(
		'#type' => 'textarea',
		'#title' => t('Description'),
		'#required' => FALSE,
	);

	$form['sirtaqui_content'] = array(
		'#prefix' => '<div id="sirtaqui-content">',
		'#suffix' => '</div>',
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
		'#title' => t('Content type(s)'),
		//'#attributes' => array('id' => 'sirtaqui-content'),
	);
	$form['sirtaqui_content']['content_type_0'] = array(
		'#type' => 'select',
		'#title' => t(''),
		'#required' => TRUE,
		'#options' => $content_type,
		'#weight' => 0,
	);
	$form['sirtaqui_content']['delete_content_type'] = array(
		'#type' => 'submit',
		'#value' => t('Delete last content type'),
		'#ahah' => array(
			'path' => 'admin/content/sirtaqui/content_listing/new/js/delete_sirtaqui_content',
			'wrapper' => 'sirtaqui-content',
			'effect' => 'fade',
			'method' => 'replace',
		),
	);
	$form['sirtaqui_content']['add_content_type'] = array(
		'#type' => 'submit',
		'#value' => t('Add content type'),
		'#ahah' => array(
			'path' => 'admin/content/sirtaqui/content_listing/new/js/add_sirtaqui_content',
			'wrapper' => 'sirtaqui-content',
			'effect' => 'fade',
			'method' => 'replace',
		),
	);

	//krumo($form);
	$form['submit'] = array('#type' => 'submit', '#value' => t('Create listing'));

	return $form;
}

/**
 * Form Submit. Create sirtaqui content listing
 *
 * @ingroup forms
 * @see sirtaqui_create_sirtaqui_content_listing()
 * @see theme_sirtaqui_create_sirtaqui_content_listing()
 */
function sirtaqui_create_sirtaqui_content_listing_submit($form, &$form_state) {

// TODO : create matching so_search engine

	$content_list = array();
	foreach ($form_state['values'] as $key => $value) {
		if (preg_match('/^content_type_[0-9]+/', $key))
			$content_list[] = $value;
	}

	// On crée le modèle dans la base
	$sirtaqui_content_listing = array(
		'id' => '',
		'content_listing_name' => $form_state['values']['content_listing_name'],
		'content_listing_type' => truncate_utf8(preg_replace('/[^a-zA-Z0-9_]/', '_', strtolower($form_state['values']['content_listing_name']), -1), FIELD_MAX_LENGTH),
		'description' => $form_state['values']['description'],
		'sirtaqui_content' => serialize($content_list),
	);

	if (!sirtaqui_add_sirtaqui_content_listing($sirtaqui_content_listing))
	{
		drupal_set_message(t('Error while saving sirtaqui content listing %content', array('%content' => $sirtaqui_content_listing['content_name'])), 'error');
		$form_state['redirect'] = "admin/content/sirtaqui/content_listing/new";
		return;
	}

    $content_listing = sirtaqui_load_sirtaqui_content_listing(db_last_insert_id('sirtaqui_content_listing', 'id'));

    if($content_listing != false) {
        module_invoke_all('scl_create', $content_listing);
    }

	$form_state['redirect'] = "admin/content/sirtaqui/content_listing/edit/".$sirtaqui_content_listing['id'];

	return;
}

/**
 * Form Theme. Create sirtaqui content listing
 *
 * @ingroup forms
 * @see sirtaqui_create_sirtaqui_content_listing()
 * @see sirtaqui_create_sirtaqui_content_listing_submit()
 */
function theme_sirtaqui_create_sirtaqui_content_listing($form) {

}

/**
 * Add a select field to the form
 */
function sirtaqui_content_listing_add_content_type_select() {

	// On récupère tous les types de contenu déjà insérés
	$content_set = array();
	foreach ($_POST as $key => $value) {
		if (preg_match('/content_type/', $key))
			$content_set[$value] = $key;
	}

	$content_type = node_get_types('names');

	if (count($content_type) > 0)
		foreach ($content_type as $type => $name) {
			if (!preg_match('/^sirtaqui_.+/', $type) || isset($content_set[$type]))
				unset($content_type[$type]);
		}

	// Récupération de l'ID unique du formulaire
	$form_build_id = $_POST['form_build_id'];

	// On fabrique un faux form_state
 	$form_state = array('submitted' => FALSE);

 	// Récupération du formulaire à partir du cache
	$form = form_get_cache($form_build_id, $form_state);

	$nb_content = count($content_set);

	if (count($content_type) == 1)
		unset($form['sirtaqui_content']['add_content_type']);

	if (count($content_type) > 0)
		$form['sirtaqui_content']['content_type_'.($nb_content-1)]['#attributes'] = array('readonly' => 'readonly');


	// On ajoute notre élément dynamique dans le formulaire
	if (count($content_type) > 0)
	{
		$form['sirtaqui_content']['content_type_'.$nb_content] = array(
			'#type' => 'select',
			'#title' => t(''),
			'#required' => TRUE,
			'#options' => $content_type,
			'#weight' => $nb_content,
		);

		$form['sirtaqui_content']['delete_content_type']['#weight'] = 1 + $nb_content;
		if (count($content_type) > 1)
			$form['sirtaqui_content']['add_content_type']['#weight'] = 2 + $nb_content;
	}

	// Sauvegarde du formulaire dans le cache
	form_set_cache($form_build_id, $form, $form_state);

	// Reconstruction du formulaire
	$form = form_builder($_POST['form_id'], $form, $form_state);

	// Récupération de notre élément reconstruit
	if (count($content_type) > 0)
		$element = $form['sirtaqui_content'];

	// Transformation de l'élément en HTML
	$output = t('');
	if (count($content_type) > 0)
		$output = drupal_render($element);

	drupal_json(array('data' => $output, 'status' => TRUE));
	exit();
}

function sirtaqui_content_listing_delete_content_type() {

	// On compte le nombre de types de contenu déjà insérés
	$nb_content = 0;
	foreach ($_POST as $key => $value) {
		if (preg_match('/content_type_[0-9]+/', $key))
			$nb_content++;
	}

	// Récupération de l'ID unique du formulaire
	$form_build_id = $_POST['form_build_id'];

	// On fabrique un faux form_state
 	$form_state = array('submitted' => FALSE);

 	// Récupération du formulaire à partir du cache
	$form = form_get_cache($form_build_id, $form_state);

	if ($nb_content > 1)
		unset($form['sirtaqui_content']['content_type_'.($nb_content-1)]);

	// Si le bouton d'ajout de contenu avait été supprimé, il est recréé
	if (empty($form['sirtaqui_content']['add_content_type']))
		$form['sirtaqui_content']['add_content_type'] = array(
			'#type' => 'submit',
			'#value' => t('Add content type'),
			'#weight' => $nb_content,
			'#ahah' => array(
				'path' => 'admin/content/sirtaqui/content_listing/new/js/add_sirtaqui_content',
				'wrapper' => 'sirtaqui-content',
				'effect' => 'fade',
				'method' => 'replace',
			),
		);

	// Sauvegarde du formulaire dans le cache
	form_set_cache($form_build_id, $form, $form_state);

	// Reconstruction du formulaire
	$form = form_builder($_POST['form_id'], $form, $form_state);

	// Récupération de notre élément reconstruit
	$element = $form['sirtaqui_content'];

	// Transformation de l'élément en HTML
	$output = drupal_render($element);

	drupal_json(array('data' => $output, 'status' => TRUE));
	exit();
}

/***
 * EDIT AN EXISTING CONTENT TYPE LISTING
 */

/**
 * Form Builder. Create sirtaqui content listing
 *
 * @ingroup forms
 * @see sirtaqui_edit_sirtaqui_content_listing_submit()
 * @see theme_sirtaqui_edit_sirtaqui_content_listing()
 */
function sirtaqui_edit_sirtaqui_content_listing(&$form_state, $ctlid) {

	$sirtaqui_content_listing = sirtaqui_load_sirtaqui_content_listing($ctlid);
	$sirtaqui_content_listing->sirtaqui_content = unserialize($sirtaqui_content_listing->sirtaqui_content);

	$content_type = node_get_types('names');

	if (count($content_type) > 0)
		foreach ($content_type as $type => $name) {
			if (!preg_match('/^sirtaqui_.*/', $type))
				unset($content_type[$type]);
		}

	$form = array();

	$form['id'] = array(
		'#type' => 'hidden',
		'#value' => $sirtaqui_content_listing->id,
	);
	$form['content_listing_type'] = array(
		'#type' => 'hidden',
		'#value' => $sirtaqui_content_listing->content_listing_type,
	);
	$form['content_listing_name'] = array(
		'#type' => 'textfield',
		'#title' => t('Listing Name'),
		'#default_value' => $sirtaqui_content_listing->content_listing_name,
		'#required' => TRUE,
	);
	$form['description'] = array(
		'#type' => 'textarea',
		'#title' => t('Description'),
		'#default_value' => $sirtaqui_content_listing->description,
		'#required' => FALSE,
	);

	$form['sirtaqui_content'] = array(
		'#prefix' => '<div id="sirtaqui-content">',
		'#suffix' => '</div>',
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
		'#title' => t('Content type(s)'),
		//'#attributes' => array('id' => 'sirtaqui-content'),
	);
	foreach ($sirtaqui_content_listing->sirtaqui_content as $index => $value) {
		$form['sirtaqui_content']['content_type_'.$index] = array(
			'#type' => 'select',
			'#title' => t(''),
			'#required' => TRUE,
			'#default_value' => $value,
			'#options' => $content_type,
			'#weight' => $index,
			'#attributes' => array('readonly' => 'readonly')
		);
	}

	$form['sirtaqui_content']['delete_content_type'] = array(
		'#type' => 'submit',
		'#value' => t('Delete last content type'),
		'#ahah' => array(
			'path' => 'admin/content/sirtaqui/content_listing/new/js/delete_sirtaqui_content',
			'wrapper' => 'sirtaqui-content',
			'effect' => 'fade',
			'method' => 'replace',
		),
	);
	$form['sirtaqui_content']['add_content_type'] = array(
		'#type' => 'submit',
		'#value' => t('Add content type'),
		'#ahah' => array(
			'path' => 'admin/content/sirtaqui/content_listing/new/js/add_sirtaqui_content',
			'wrapper' => 'sirtaqui-content',
			'effect' => 'fade',
			'method' => 'replace',
		),
	);

	//krumo($form);
	$form['submit'] = array('#type' => 'submit', '#value' => t('Save listing'));

	return $form;
}

/**
 * Form Builder. Create sirtaqui content listing
 *
 * @ingroup forms
 * @see sirtaqui_edit_sirtaqui_content_listing()
 * @see theme_sirtaqui_edit_sirtaqui_content_listing()
 */
function sirtaqui_edit_sirtaqui_content_listing_submit($form, &$form_state) {

    $sctl = sirtaqui_load_sirtaqui_content_listing($form_state['values']['id']);

	$content_list = array();
	foreach ($form_state['values'] as $key => $value) {
		if (preg_match('/^content_type_[0-9]+/', $key))
			$content_list[] = $value;
	}

    $sirtaqui_content = unserialize($sctl->sirtaqui_content);
    $filters = unserialize($sctl->filters);
    $cache = unserialize($sctl->cache);

	if($content_list != $sirtaqui_content) {

        // un content type déjà présent a t-il été supprimé ou modifié ?
        $removeds = array_diff($sirtaqui_content, $content_list);

        if(!empty($removeds)) {
            $filters['nodes_filter'] = array();
            $cache['advanced_filters'] = array();

            foreach($removeds as $removed) {
                unset($filters['filters'][$removed]);
                unset($filters['advanced_filters'][$removed]);
            }
        }
    }

	// On crée le modèle dans la base
	$sirtaqui_content_listing = array(
		'id' => $form_state['values']['id'],
		'content_listing_name' => $form_state['values']['content_listing_name'],
		'content_listing_type' => $form_state['values']['content_listing_type'],
		'description' => $form_state['values']['description'],
		'sirtaqui_content' => serialize($content_list),
		'filters' => serialize($filters),
		'cache' => serialize($cache),
        'sorts' => $sctl->sorts,
	);

	if (!sirtaqui_add_sirtaqui_content_listing($sirtaqui_content_listing))
	{
		drupal_set_message(t('Error while saving sirtaqui content listing %content', array('%content' => $sirtaqui_content_listing['content_name'])), 'error');
		$form_state['redirect'] = "admin/content/sirtaqui/content_listing/new";
		return;
	}

	$form_state['redirect'] = "admin/content/sirtaqui/content_listing/edit/".$sirtaqui_content_listing['id'];

	return;
}

/**
 * Form Builder. Create sirtaqui content listing
 *
 * @ingroup forms
 * @see sirtaqui_edit_sirtaqui_content_listing()
 * @see sirtaqui_edit_sirtaqui_content_listing_submit()
 */
function theme_sirtaqui_edit_sirtaqui_content_listing() {

}

/***
 * SORTING
 */

/**
 * Sorting edit form
 */
function sirtaqui_content_listing_edit_sorting(&$form_state, $clid) {

    $form = array();

    $form['#tree'] = true;

    $form['#content_listing_infos'] = array('clid' => $clid);

    $content_listing = sirtaqui_load_sirtaqui_content_listing($clid);
    $content_types = unserialize($content_listing->sirtaqui_content);
    $sorts = unserialize($content_listing->sorts);

    $form['sorting'] = array(
        '#type' => 'fieldset',
        '#title' => t("Sorting"),
        '#collapsible' => !empty($form_state['storage']),
        '#collapsed' => !empty($form_state['storage']),
    );

    $shared_fields = array();
    $temp_fields = array();
    $fields = array();

    // on consitue un tableau des champs communs à tous les types de contenus
    foreach($content_types as $content_type) {
        $tmp = sirtaqui_content_listing_form_fields_helper($content_type);

        if (empty($shared_fields)) {
            $shared_fields = $tmp;
        } else {
            $shared_fields = array_intersect_assoc($tmp, $shared_fields);
        }
    }

    // on constitue la liste provisoire de tous les champs
    foreach($content_types as $content_type) {
        $tmp = sirtaqui_content_listing_form_fields_helper($content_type, $form['#content_listing_infos']['fields']);

        $temp_fields = array_merge($temp_fields, $tmp);
    }

    asort($temp_fields);

    // on constitue la liste définitive de tous les champs, et on marque les champs en commun
    foreach($temp_fields as $key => $value) {
        $fields[$key] = (isset($shared_fields[$key]) == true ? '*' : '') . $value;
    }

    $preview_fields = $fields;

    array_unshift($fields, "- " . t("Available fields") . " -");

    $form['sorting']['available_fields'] = array(
        '#type' => 'select',
        '#title' => t("Fields helper"),
        '#options' => $fields,
        '#attributes' => array('class' => 'fields_helper'),
        '#description' => t("* shared fields"),
    );

    $form['sorting']['field_name_helper'] = array(
        '#type' => 'textfield',
        '#attributes' => array('class' => 'field_name_helper'),
    );

    $form['sorting']['field_f_name_helper'] = array(
        '#type' => 'textfield',
        '#attributes' => array('class' => 'field_f_name_helper'),
    );

    $form['sorting']['sorts_fragment'] = array(
        '#type' => 'textarea',
        '#title' => t("'ORDER BY' clause fragment"),
        '#wysiwyg' => false,
        '#default_value' => !empty($form_state['values']['sorting']['sorts_fragment']) ? $form_state['values']['sorting']['sorts_fragment'] : $sorts['order_by'],
        '#description' => t("This fragment will be inserted in query, as this :
                <br />ORDER BY <strong>[THE FRAGMENT]</strong>"),
    );

    $form['preview_settings'] = array(
        '#type' => 'fieldset',
        '#title' => t("Preview"),
        '#collapsible' => true,
        '#collapsed' => true,
    );

    global $language;

    $language_options = array_combine(array_keys(language_list()), array_keys(language_list()));

    $language_options = array_merge(array(
        '0' => t('all'),
    ), $language_options);

    $form['preview_settings']['language'] = array(
        '#type' => 'select',
        '#title' => t("Language filter"),
        '#options' => $language_options,
        '#default_value' => isset($form_state['values']['preview_settings']['language']) ? $form_state['values']['preview_settings']['language'] : $language->language,
    );

    $form['preview_settings']['fields'] = array(
        '#type' => 'select',
        '#title' => t("Fields to display in preview, in addition to 'nid', 'title' and 'type'"),
        '#options' => $preview_fields,
        '#multiple' => true,
        '#size' => 10,
        '#default_value' => $form_state['values']['preview_settings']['fields'],
        '#description' => t("* shared fields"),
    );

    $form['preview_settings']['extract'] = array(
        '#type' => 'textfield',
        '#title' => t("Additional extractions"),
        '#size' => 100,
        '#description' => t("More fields to select, with or without aliases, separated with commas."),
        '#default_value' => $form_state['values']['preview_settings']['extract'],
    );

    $form['preview_settings']['limit'] = array(
        '#type' => 'textfield',
        '#title' => t("Number of results"),
        '#size' => 4,
        '#default_value' => !empty($form_state['values']['preview_settings']['limit']) ? $form_state['values']['preview_settings']['limit'] : 50,
    );

    $form['preview_settings']['use_filters'] = array(
        '#type' => 'checkbox',
        '#title' => t("Use user filters"),
        '#return_value' => 1,
        '#default_value' => isset($form_state['values']['preview_settings']['use_filters']) ? $form_state['values']['preview_settings']['use_filters'] : 1,
    );

    $form['preview_settings']['use_advanced_filters'] = array(
        '#type' => 'checkbox',
        '#title' => t("Use advanced filters"),
        '#return_value' => 1,
        '#default_value' => isset($form_state['values']['preview_settings']['use_advanced_filters']) ? $form_state['values']['preview_settings']['use_advanced_filters'] : 1,
    );

    $form['preview_settings']['use_hook_query'] = array(
        '#type' => 'checkbox',
        '#title' => t("Use calls to hook_scl_query()"),
        '#return_value' => 1,
        '#default_value' => isset($form_state['values']['preview_settings']['use_hook_query']) ? $form_state['values']['preview_settings']['use_hook_query'] : 1,
    );

    $form['preview_settings']['preview'] = array(
        '#type' => 'submit',
        '#value' => t("Try the sort"),
    );

    $form['save'] = array(
        '#type' => 'submit',
        '#value' => t("Publish the sort"),
        '#printed' => true, // on "dissimule" le bouton afin de pouvoir décider dans la fonction de thème, si on l'affiche ou non.
    );

    if(!empty($form_state['storage'])) {

        $error = "";

        set_error_handler(function($code, $msg) use (&$error) {
            $error = $msg;
        }, E_ALL & ~E_NOTICE | E_STRICT);

        if($form_state['values']['preview_settings']['language'] !== '0') {
            i18n_selection_mode('custom', "%alias.language ='" . $form_state['values']['preview_settings']['language'] . "' OR %alias.language ='' OR %alias.language IS NULL");
        }

        $form['#preview_fragment'] = $form_state['storage']['fragment'];
        $form['#preview_joins'] = $form_state['storage']['joins'];

        $query = $form_state['storage']['query'];

        $form['#preview_extractions'] = scl_compile_listing($clid, array(
            'extract' => array($form_state['storage']['extract']),
            'paginate' => false,
            'filters' => $form_state['storage']['use_filters'],
            'advanced_filters' => $form_state['storage']['use_advanced_filters'],
            'sorting' => true,
            'limit' => $form_state['storage']['limit'],
            'search' => false, // on ne veut pas fausser les résultats
            'hook_query' => $form_state['storage']['use_hook_query'],
            'ignore_post' => true, // pertinent, car on travail avec un formulaire soumis mais rebuilded donc non-redirigé
        ), $query);

        $form['#preview_fields'] = $form_state['storage']['fields'];
        $form['#preview_error'] = $error;
        $form['#preview_query'] = empty($error) ? $query : '';

        if($form_state['values']['preview_settings']['language'] !== '0') {
            i18n_selection_mode('reset');
        }

        restore_error_handler();
    }

    unset($form_state['storage']);

    return $form;
}

/**
 * Sorting edit form submit callback
 */
function sirtaqui_content_listing_edit_sorting_submit($form, &$form_state) {

    $query = sirtaqui_content_listing_init_query();

    $order_fragment = $form_state['values']['sorting']['sorts_fragment'];

    $fields = $form['#content_listing_infos']['fields'];

    $order_fields = array();
    $index_replacement = 0;

    // on doit effectuer les remplacements en deux temps, pour ne pas réécrire des champs déjà réécrits
    $order_fragment = preg_replace_callback('#\b(field_[a-z0-9_]+)(?:_value|_value2|_fid)\b#',
        function($element) use (&$order_fields, &$index_replacement, $fields) {
            $index = '$' . $index_replacement++;

            // une petite sécurité, pour se prémunir (en partie) d'une erreur de capture de l'expreg
            if(!in_array($element[1], $fields)) {
                return $element[0];
            }

            $order_fields[$index] = $element; // [0] : nom du champ dans la table, [1] : nom du champ CCK

            return $index;
        },
    $order_fragment);

    $taxo_joins = array();

    foreach($order_fields as $replacement => $field) {
        $query_fragment = sirtaqui_content_listing_prepare_query_fragment_data($field[1] . '@sirtaqui', $content_type); // TODO : $content_type ???

        // on ne prends que le champ qui nous intéresse
        $field_index = array_search($field[0], array_keys($query_fragment->infos));

        $query_field = $query_fragment->fields[$field_index];

        $query['join'] = array_merge($query['join'], $query_fragment->joins);

        // champs de taxonomy
        // on joint la table {term_data} en lui générant un alias propre à ce champs : nom du champ + '_taxo'
        if($query_fragment->infos[$field[0]]['type'] == 'content_taxonomy') {

            $join = "LEFT JOIN {term_data} AS " . $field[1] . "_taxo ON " . $field[1] . "_taxo.tid=" . $query_fragment->fields[0];
            $query['join'][] = $join;
            $taxo_joins[] = $join;

            $query_field = $field[1] . "_taxo.name";
        }

        $order_fragment = str_replace($replacement, $query_field, $order_fragment);
    }

    $query['order_by'] = array($order_fragment);

    switch(array_pop($form_state['clicked_button']['#parents'])) {
        case 'preview':

            $form_state['storage']['extract'] = $form_state['values']['preview_settings']['extract'];
            $form_state['storage']['limit'] = $form_state['values']['preview_settings']['limit'];
            $form_state['storage']['query'] = $query;
            $form_state['storage']['fragment'] = $order_fragment;
            $form_state['storage']['joins'] = implode('<br />', $taxo_joins);
            $form_state['storage']['fields'] = $form_state['values']['preview_settings']['fields'];
            $form_state['storage']['use_filters'] = $form_state['values']['preview_settings']['use_filters'];
            $form_state['storage']['use_advanced_filters'] = $form_state['values']['preview_settings']['use_advanced_filters'];
            $form_state['storage']['use_hook_query'] = $form_state['values']['preview_settings']['use_hook_query'];

            $form_state['rebuild'] = true;

            break;

        case 'save':
            $content_listing = sirtaqui_load_sirtaqui_content_listing($form['#content_listing_infos']['clid']);
            $sorts = unserialize($content_listing->sorts);
            $cache = unserialize($content_listing->cache);

            $sorts['order_by'] = $form_state['values']['sorting']['sorts_fragment'];
            $cache['sorts'] = array_filter($query);

            $content_listing->sorts = serialize($sorts);
            $content_listing->cache = serialize($cache);

            $content_listing = (array)$content_listing; // obligé de convertir, pour la fonction
            sirtaqui_add_sirtaqui_content_listing($content_listing);

            drupal_set_message(t("Sorting has been saved"), 'status');

            break;
    }
}

/**
 * Theme for sorting edit form
 */
function theme_sirtaqui_content_listing_edit_sorting($form) {

    $output = drupal_render($form);

    if(!empty($form['#preview_query'])) {
        $output .= "<h2>" . t("Generated query") . " : </h2>";
        $output .= "<div class='ok'>" . check_plain($form['#preview_query']) . "</div>";
    }

    if(!empty($form['#preview_error'])) {
        $output .= "<h2>" . t("Oops ! The query has a problem") . " : </h2>";
        $output .= "<div class='warning'>" . $form['#preview_error'] . "</div>";

        return $output;
    }

    if(!empty($form['#preview_fragment'])) {
        $output .= "<h2>" . t("Generated query fragment") . " : </h2>";
        $output .= "<div class='ok'>" . check_plain($form['#preview_fragment']) . "</div>";

        if(!empty($form['#preview_joins'])) {
            $output .= "<h2>" . t("Generated query JOIN") . " : </h2>";
            $output .= "<div class='ok'>" . $form['#preview_joins'] . "</div>";
        }

    } elseif(!isset($form['#preview_fragment'])) {
        return $output;
    }

    unset($form['save']['#printed']); // on réactive le bouton
    $output .= drupal_render($form['save']);

    $output .= sirtaqui_content_listing_nodes_preview($form['#preview_extractions'], $form['#preview_fields']);

    return $output;
}

/***
 * FILTERS
 */

function sirtaqui_content_listing_cck_fields_helper($cck_data_type) {
    $types = array(
        'text' => '_value',
        'content_taxonomy' => '_value',
        'number_decimal' => '_value',
        'number_integer' => '_value',
        'number_float' => '_value',
        'date' => array('_value', '_value2'),
        'datestamp' => array('_value', '_value2'),
        'datetime' => array('_value', '_value2'),
        'filefield' => '_fid',
        'nodereference' => '_value',
    );

    return $types[$cck_data_type];
}

function sirtaqui_content_listing_form_fields_helper($content_type, &$container = array()) {

    $content_type_infos = content_types($content_type);

    $fields = array();

    foreach($content_type_infos['fields'] as $name => $field) {
        if(preg_match('#^field_f_#', $name)) {continue;}

        $field_infos = content_fields($name, $content_type);

        $index = $name;
        $field_f_name = normalize_field_name(str_replace('field_', 'field_f_', $name));

        $index .= '|' . (isset($content_type_infos['fields'][$field_f_name]) && $field_f_name != $name ? $field_f_name : '');
        $index .= '|' . implode('|', (array)sirtaqui_content_listing_cck_fields_helper($field_infos['type']));

        $fields[$index] = $field['widget']['label'];

        $container[] = $name;
        $container[] = $field_f_name;
    }

    return $fields;
}

/**
 * Advanced filters edit form
 */
function sirtaqui_content_listing_edit_advanced_filters(&$form_state, $clid) {

    $form = array();

    $form['#tree'] = true;

    $form['#content_listing_infos'] = array('clid' => $clid);

    $content_listing = sirtaqui_load_sirtaqui_content_listing($clid);
    $content_types = unserialize($content_listing->sirtaqui_content);
    $filters = unserialize($content_listing->filters);

    $form['#content_listing_infos']['content_types'] = $content_types;

    $preview_fields = array();

    foreach($content_types as $content_type) {
        $form['content_type_' . $content_type] = array(
            '#type' => 'fieldset',
            '#title' => t("Filter for") . " '" . $content_type . "'",
            '#collapsible' => !empty($form_state['storage']),
            '#collapsed' => !empty($form_state['storage']),
        );

        $form['#content_listing_infos'][$content_type . '_fields'] = array();

        // on en profite pour passer les champs au submit
        $fields = sirtaqui_content_listing_form_fields_helper($content_type, $form['#content_listing_infos'][$content_type . '_fields']);

        // pour les champs de preview, on ne prend que les champs communs à tous les types de contenus concernés
        if (empty($preview_fields)) {
            $preview_fields = $fields;
        } else {
            $preview_fields = array_intersect_assoc($fields, $preview_fields);
        }

        asort($fields);

        array_unshift($fields, "- " . t("Available fields") . " -");

        $form['content_type_' . $content_type]['available_fields'] = array(
            '#type' => 'select',
            '#title' => t("Fields helper"),
            '#options' => $fields,
            '#attributes' => array('class' => 'fields_helper'),
        );

        $form['content_type_' . $content_type]['field_name_helper'] = array(
            '#type' => 'textfield',
            '#attributes' => array('class' => 'field_name_helper'),
        );

        $form['content_type_' . $content_type]['field_f_name_helper'] = array(
            '#type' => 'textfield',
            '#attributes' => array('class' => 'field_f_name_helper'),
        );

        $form['content_type_' . $content_type]['where_fragment'] = array(
            '#type' => 'textarea',
            '#title' => t("'WHERE' clause fragment"),
            '#wysiwyg' => false,
            '#description' => t("Think to include fields '_f' as well !
                <br />This fragment will be inserted in query, as this :
                <br />WHERE ( (n.type='<strong>%content_type</strong>' AND (<strong>[THE FRAGMENT]</strong>)) OR (n.type='<em>another_content_type</em>' AND (<em>[another fragment]</em>)) OR ... ) AND <em>other query stuff</em>...",
                array('%content_type' => $content_type)
            ),
            '#default_value' => !empty($form_state['values']['content_type_' . $content_type]['where_fragment']) ? $form_state['values']['content_type_' . $content_type]['where_fragment'] : $filters['advanced_filters'][$content_type],
        );
    }

    $form['preview_settings'] = array(
        '#type' => 'fieldset',
        '#title' => t("Preview"),
        '#collapsible' => true,
        '#collapsed' => true,
    );

    global $language;

    $language_options = array_combine(array_keys(language_list()), array_keys(language_list()));

    $language_options = array_merge(array(
        '0' => t('all'),
    ), $language_options);

    $form['preview_settings']['language'] = array(
        '#type' => 'select',
        '#title' => t("Language filter"),
        '#options' => $language_options,
        '#default_value' => isset($form_state['values']['preview_settings']['language']) ? $form_state['values']['preview_settings']['language'] : $language->language,
    );

    asort($preview_fields);

    $form['preview_settings']['fields'] = array(
        '#type' => 'select',
        '#title' => t("Fields to display in preview, in addition to 'nid', 'title' and 'type'"),
        '#options' => $preview_fields,
        '#multiple' => true,
        '#size' => 10,
        '#default_value' => $form_state['values']['preview_settings']['fields'],
    );

    $form['preview_settings']['extract'] = array(
        '#type' => 'textfield',
        '#title' => t("Additional extractions"),
        '#size' => 100,
        '#description' => t("More fields to select, with or without aliases, separated with commas."),
        '#default_value' => $form_state['values']['preview_settings']['extract'],
    );

    $form['preview_settings']['limit'] = array(
        '#type' => 'textfield',
        '#title' => t("Number of results"),
        '#size' => 4,
        '#default_value' => !empty($form_state['values']['preview_settings']['limit']) ? $form_state['values']['preview_settings']['limit'] : 50,
    );

    $form['preview_settings']['use_filters'] = array(
        '#type' => 'checkbox',
        '#title' => t("Use user filters"),
        '#return_value' => 1,
        '#default_value' => isset($form_state['values']['preview_settings']['use_filters']) ? $form_state['values']['preview_settings']['use_filters'] : 1,
    );

    $form['preview_settings']['use_sorting'] = array(
        '#type' => 'checkbox',
        '#title' => t("Use sorting"),
        '#return_value' => 1,
        '#default_value' => isset($form_state['values']['preview_settings']['use_sorting']) ? $form_state['values']['preview_settings']['use_sorting'] : 0,
    );

    $form['preview_settings']['preview'] = array(
        '#type' => 'submit',
        '#value' => t("Try the filters"),
    );

    $form['save'] = array(
        '#type' => 'submit',
        '#value' => t("Publish the filters"),
        '#printed' => true, // on "dissimule" le bouton afin de pouvoir décider dans la fonction de thème, si on l'affiche ou non.
    );

    if(!empty($form_state['storage'])) {

        $error = "";

        set_error_handler(function($code, $msg) use (&$error) {
            $error = $msg;
        }, E_ALL & ~E_NOTICE | E_STRICT);

        if($form_state['values']['preview_settings']['language'] !== '0') {
            i18n_selection_mode('custom', "%alias.language ='" . $form_state['values']['preview_settings']['language'] . "' OR %alias.language ='' OR %alias.language IS NULL");
        }

        $form['#preview_fragment'] = $form_state['storage']['fragment'];

        $query = $form_state['storage']['query'];

        $form['#preview_extractions'] = scl_compile_listing($clid, array(
            'extract' => array($form_state['storage']['extract']),
            'paginate' => false,
            'filters' => $form_state['storage']['use_filters'],
            'advanced_filters' => false, // on ne doit pas utiliser la requête en cache
            'sorting' => $form_state['storage']['use_sorting'],
            'limit' => $form_state['storage']['limit'],
            'search' => false, // on ne veut pas fausser les résultats
            'ignore_post' => true, // pertinent, car on travail avec un formulaire soumis mais rebuilded donc non-redirigé
        ), $query);

        $form['#preview_fields'] = $form_state['storage']['fields'];
        $form['#preview_error'] = $error;
        $form['#preview_query'] = empty($error) ? $query : '';

        if($form_state['values']['preview_settings']['language'] !== '0') {
            i18n_selection_mode('reset');
        }

        restore_error_handler();
    }

    unset($form_state['storage']);

    return $form;
}

/**
 * Advanced filters edit form submit callback
 */
function sirtaqui_content_listing_edit_advanced_filters_submit($form, &$form_state) {

    $where_clause = array();

    $query = sirtaqui_content_listing_init_query();

    foreach($form['#content_listing_infos']['content_types'] as $content_type) {

        $where_fragment = $form_state['values']['content_type_' . $content_type]['where_fragment'];

        if($where_fragment == null) {$where_fragment = "1";}

        $fields = array_filter($form['#content_listing_infos'][$content_type . '_fields']);

        $where_fields = array();
        $index_replacement = 0;

        // on doit effectuer les remplacements en deux temps, pour ne pas réécrire des champs déjà réécrits
        $where_fragment = preg_replace_callback('#\b(field_[a-z0-9_]+)(?:_value|_value2|_fid)\b#',
            function($element) use (&$where_fields, &$index_replacement, $fields) {
                $index = '$' . $index_replacement++;

                // une petite sécurité, pour se prémunir (en partie) d'une erreur de capture de l'expreg
                if(!in_array($element[1], $fields)) {
                    return $element[0];
                }

                $where_fields[$index] = $element; // [0] : nom du champ dans la table, [1] : nom du champ CCK

                return $index;
            },
        $where_fragment);

        foreach($where_fields as $replacement => $field) {
            $query_fragment = sirtaqui_content_listing_prepare_query_fragment_data($field[1] . '@sirtaqui', $content_type);

            $query['join'] = array_merge($query['join'], $query_fragment->joins);

            // on ne prends que le champ qui nous intéresse
            $field_index = array_search($field[0], array_keys($query_fragment->infos));

            $where_fragment = str_replace($replacement, $query_fragment->fields[$field_index], $where_fragment);
        }

        $where_clause[] = "(n.type='" . $content_type . "' AND (" . $where_fragment . "))";
    }

    $compiled_where_clause = "(" . implode(' OR ', $where_clause) . ")";
    $query['where'][] = $compiled_where_clause;

    switch(array_pop($form_state['clicked_button']['#parents'])) {
        case 'preview':

            $form_state['storage']['extract'] = $form_state['values']['preview_settings']['extract'];
            $form_state['storage']['limit'] = $form_state['values']['preview_settings']['limit'];
            $form_state['storage']['query'] = $query;
            $form_state['storage']['fragment'] = $compiled_where_clause;
            $form_state['storage']['fields'] = $form_state['values']['preview_settings']['fields'];
            $form_state['storage']['use_filters'] = $form_state['values']['preview_settings']['use_filters'];
            $form_state['storage']['use_sorting'] = $form_state['values']['preview_settings']['use_sorting'];

            $form_state['rebuild'] = true;

            break;

        case 'save':

            $advanced_filters = array();

            foreach($form['#content_listing_infos']['content_types'] as $content_type) {
                $advanced_filters[$content_type] = $form_state['values']['content_type_' . $content_type]['where_fragment'];
            }

            $content_listing = sirtaqui_load_sirtaqui_content_listing($form['#content_listing_infos']['clid']);
            $filters = unserialize($content_listing->filters);
            $cache = unserialize($content_listing->cache);

            $filters['advanced_filters'] = $advanced_filters;
            $cache['advanced_filters'] = array_filter($query);

            $content_listing->filters = serialize($filters);
            $content_listing->cache = serialize($cache);

            $content_listing = (array)$content_listing; // obligé de convertir, pour la fonction
            sirtaqui_add_sirtaqui_content_listing($content_listing);

            drupal_set_message(t("Filters have been saved"), 'status');

            break;
    }

}

/**
 * Theme for nodes filter fieldset content
 */
function theme_sirtaqui_content_listing_nodes_filter($element) {

    $output = "";
    
    $rows = array();

    $tnids = $element['tnids'];
    unset($element['tnids']);
    
    if($tnids['#num_selected'] > 0) {
        $output .= "<div><strong>" . format_plural($tnids['#num_selected'], '1 node selected', '@count nodes selected') . 
                    " " . t("on @count", array('@count' => count(element_children($tnids)))) . "</strong></div>";
    }

    foreach(element_children($element) as $nid) {
        $row = array();

        $node = $element[$nid];

        $row[] = drupal_render($tnids[$node['#tnid']]);
        $row[] = drupal_render($node['title']);
        $row[] = drupal_render($node['fnid']);
        $row[] = drupal_render($node['type']);
        $row[] = drupal_render($node['status']);

        $rows[] = array('data' => $row, 'class' => ($node['#outline'] ? 'ok' : ''));
    }

    $headers = array(theme('table_select_header_cell'), t("Title"), "#ID", t("Type"), t("Status"));

    $output .= theme('table', $headers, $rows, array('class' => 'system-status-report'));
    
    return $output;
}

/**
 * Theme for advanced filters edit form
 */
function theme_sirtaqui_content_listing_edit_advanced_filters($form) {

    $output = drupal_render($form);

    if(!empty($form['#preview_query'])) {
        $output .= "<h2>" . t("Generated query") . " : </h2>";
        $output .= "<div class='ok'>" . check_plain($form['#preview_query']) . "</div>";
    }

    if(!empty($form['#preview_error'])) {
        $output .= "<h2>" . t("Oops ! The query has a problem") . " : </h2>";
        $output .= "<div class='warning'>" . $form['#preview_error'] . "</div>";

        return $output;
    }

    if(!empty($form['#preview_fragment'])) {
        $output .= "<h2>" . t("Generated query fragment") . " : </h2>";
        $output .= "<div class='ok'>" . check_plain($form['#preview_fragment']) . "</div>";
    } else {
        return $output;
    }

    unset($form['save']['#printed']); // on réactive le bouton
    $output .= drupal_render($form['save']);

    $output .= sirtaqui_content_listing_nodes_preview($form['#preview_extractions'], $form['#preview_fields']);

    return $output;
}

// TODO : FONCTION A REECRIRE ! Tout-ça, c'est pourri : c'est lourd, pas élégant, incompréhensible
function sirtaqui_content_listing_nodes_preview($extracted, $fields) {

    if(empty($extracted)) {
        return "<div class='warning'>" . t("- No result -") . "</div>";
    }

    $output .= "<h2>" . t("Listing preview") . " : </h2>";

    $rows = array();

    $header = array_keys(current($extracted));

    $header[] = 'title';
    $header[] = 'type';

    $taxo_fields = array();

    foreach($fields as $field_key => $field) { // champ-non-f|[champ-f]|valeur1[|valeur2]
        $fields[$field_key] = explode('|', $field);

        $header[] = $fields[$field_key][0] . $fields[$field_key][2];
        if(!empty($fields[$field_key][3])) {$header[] = $fields[$field_key][0] . $fields[$field_key][3];}
        if(!empty($fields[$field_key][1])) {$header[] = $fields[$field_key][1] . $fields[$field_key][2];}
        if(!empty($fields[$field_key][1]) && !empty($fields[$field_key][3])) {$header[] = $fields[$field_key][1] . $fields[$field_key][3];}

        // on en profite pour identifier les champs de taxonomy
        $field_infos = content_fields($fields[$field_key][0]);

        if($field_infos['type'] == 'content_taxonomy') {
            $taxo_fields[$field_key] = true;
        }
    }

    foreach($extracted as $extract) {
        $node = node_load($extract['nid']);

        $row = array();

        foreach($extract as $extracted_value) {
            $row[] = $extracted_value;
        }

        $row[] = l($node->title, $node->path, array('attributes' => array('target' => '_blank')));
        $row[] = $node->type;

        foreach($fields as $field_key => $field) {

            $values = $node->{$field[0]};
            $tmp = array();

            foreach($values as $key => $value) {
                if(isset($taxo_fields[$field_key])) {
                    $term = taxonomy_get_term($value[substr($field[2], 1)]);
                    $term_name = !empty($term) ? " (" . $term->name . ")" : "";
                } else {
                    $term_name = "";
                }

                $tmp[] = $key . ' - ' . $value[substr($field[2], 1)] . $term_name;
            }
            $row[] = implode('<br />', $tmp);

            if(!empty($field[3])) {
                $values = $node->{$field[0]};
                $tmp = array();

                foreach($values as $key => $value) {
                    $tmp[] = $key . ' - ' . $value[substr($field[3], 1)];
                }
                $row[] = implode('<br />', $tmp);
            }

            if(!empty($field[1])) {
                $values = $node->{$field[1]};
                $tmp = array();

                foreach($values as $key => $value) {
                    if(isset($taxo_fields[$field_key])) {
                        $term = taxonomy_get_term($value[substr($field[2], 1)]);
                        $term_name = !empty($term) ? " (" . $term->name . ")" : "";
                    } else {
                        $term_name = "";
                    }

                    $tmp[] = $key . ' - ' . $value[substr($field[2], 1)] . $term_name;
                }
                $row[] = implode('<br />', $tmp);
            }

            if(!empty($field[3])) {
                $values = $node->{$field[1]};
                $tmp = array();

                foreach($values as $key => $value) {
                    $tmp[] = $key . ' - ' . $value[substr($field[3], 1)];
                }
                $row[] = implode('<br />', $tmp);
            }
        }

        $rows[] = $row;
    }

    $output .= theme('table', $header, $rows);

    return $output;
}

/**
 * Form Builder. Edit sirtaqui content listing filters
 *
 * @ingroup forms
 * @see sirtaqui_edit_filters_sirtaqui_content_listing_submit()
 * @see theme_sirtaqui_content_listing_filter_field()
 */
function sirtaqui_edit_filters_sirtaqui_content_listing(&$form_state, $ctlid) {

	$sirtaqui_content_listing = sirtaqui_load_sirtaqui_content_listing($ctlid);
	$sirtaqui_content_listing->sirtaqui_content = unserialize($sirtaqui_content_listing->sirtaqui_content);
	$sirtaqui_content_listing->filters = unserialize($sirtaqui_content_listing->filters);

	$form = array();
	foreach ($sirtaqui_content_listing->sirtaqui_content as $content_type_name) {

		$content_type = content_types($content_type_name);
		$nb_filter = 0;

		// Le filtre ne se fera que sur les champs non Sirtaqui
		$fields = array('' => t('-- Select a value --'));
		foreach ($content_type['fields'] as $name => $field) {
			if (preg_match('#^field_(?!f_)#', $name) && ($field['type'] == 'text' || $field['type'] == 'content_taxonomy'))
				$fields[$name] = $field['widget']['label'];
		}

		$form['id'] = array(
			'#type' => 'hidden',
			'#title' => t(''),
			'#value' => $ctlid,
		);
		$form[$content_type_name] = array(
			'#type' => 'fieldset',
			'#title' => t('Filters for') . " " . $content_type['name'],
			'#theme' => 'sirtaqui_content_listing_filter_field',
			'#collapsible' => TRUE
		);
		$form[$content_type_name]['content_type_'.$content_type_name] = array(
			'#type' => 'hidden',
			'#title' => t(''),
			'#value' => $content_type_name,
		);

        asort($fields);

		if (isset($sirtaqui_content_listing->filters['filters'][$content_type_name]))
			foreach ($sirtaqui_content_listing->filters['filters'][$content_type_name] as $field_name => $field_value) {

				$form[$content_type_name]['filter_'.$nb_filter] = array(
					'#type' => 'markup',
					'#title' => t(''),
					'#attributes' => array('id' => $content_type_name.'_filter_'.$nb_filter)
				);
				$form[$content_type_name]['filter_'.$nb_filter][$content_type_name.'_filter_fields_'.$nb_filter] = array(
					'#type' => 'select',
					'#title' => t(''),
					'#options' => $fields,
					'#default_value' => $field_name,
					'#weight' => count($content_set),
					'#ahah' => array(
						'path' => 'admin/content/sirtaqui/content_listing/edit/js/update_filter_input/'.$content_type_name.'/'.$nb_filter,
						'wrapper' => $content_type_name.'_filter_fields_'.$nb_filter.'_input-wrapper',
						'effect' => 'fade',
						'method' => 'replace',
					),
				);
				$form[$content_type_name]['filter_'.$nb_filter][$content_type_name.'_filter_fields_'.$nb_filter.'_input'] = array(
					'#type' => 'textfield',
					'#title' => t(''),
					'#default_value' => $field_value,
					'#weight' => count($content_set),
					'#length' => 1024,
					'#size' => 40,
					'#id' => $content_type_name.'_filter_fields_'.$nb_filter.'_input'
				);

				$field_info = content_fields($field_name);
				if ($field_info['type'] == 'content_taxonomy')
					$form[$content_type_name]['filter_'.$nb_filter][$content_type_name.'_filter_fields_'.$nb_filter.'_input']['#autocomplete_path'] = 'taxonomy/autocomplete/'.$field_info['vid'];
				else
					$form[$content_type_name]['filter_'.$nb_filter][$content_type_name.'_filter_fields_'.$nb_filter.'_input']['#autocomplete_path'] = 'admin/content/sirtaqui/content_listing/edit/js/autocomplete_field/'.$content_type_name.'/'.$field_name;

				$nb_filter++;
			}

		$form[$content_type_name]['filter_'.$nb_filter] = array(
			'#type' => 'markup',
			'#title' => t(''),
			'#attributes' => array('id' => $content_type_name.'_filter_'.$nb_filter)
		);
		$form[$content_type_name]['filter_'.$nb_filter][$content_type_name.'_filter_fields_'.$nb_filter] = array(
			'#type' => 'select',
			'#title' => t(''),
			'#options' => $fields,
			'#weight' => count($content_set),
			'#ahah' => array(
				'path' => 'admin/content/sirtaqui/content_listing/edit/js/update_filter_input/'.$content_type_name.'/'.$nb_filter,
				'wrapper' => $content_type_name.'_filter_fields_'.$nb_filter.'_input-wrapper',
				'effect' => 'fade',
				'method' => 'replace',
			),
		);
		$form[$content_type_name]['filter_'.$nb_filter][$content_type_name.'_filter_fields_'.$nb_filter.'_input'] = array(
			'#type' => 'textfield',
			'#title' => t(''),
			'#weight' => count($content_set),
			'#length' => 1024,
			'#size' => 40,
			'#id' => $content_type_name.'_filter_fields_'.$nb_filter.'_input',
			'#attributes' => array('readonly' => 'readonly')
		);
		$form[$content_type_name]['add_filter'] = array(
			'#type' => 'submit',
			'#value' => t('Add filter')
		);

	}

    $form['nodes_filter'] = array(
        '#type' => 'fieldset',
        '#title' => t("Nodes filter"),
        '#description' => t("If one node or more are selected, only these nodes will be displayed in listing.<br />If nothing is selected all nodes will be displayed."),
        '#collapsible' => true,
        '#collapsed' => empty($sirtaqui_content_listing->filters['nodes_filter']),
    );

    $query = sirtaqui_content_listing_init_query();

    $query['where'][] = "n.language='%s'";
    $query['args'][] = language_default('language');

    $query['order_by'] = array('n.title ASC', 'n.language DESC');

    $nodes = scl_compile_listing($ctlid, array(
        'extract' => array(
            'n.tnid',
            'n.title',
            'fnid',
            'n.type',
            'n.status',
        ),
        'status' => null, // on veut voir tous les status
        'nodes_filter' => false,
        'search' => false,
        'hook_query' => false,
        'paginate' => false,
        'ignore_post' => true,
    ), $query);

    $form['nodes_filter']['node_list'] = array(
        '#type' => 'markup',
        '#prefix' => "<div>",
        '#suffix' => "</div>",
        '#theme' => 'sirtaqui_content_listing_nodes_filter',
    );

    $tnids_options = array();

    foreach($nodes as $node) {
        $tnid = ($node['tnid'] != 0 ? $node['tnid'] : $node['nid']);

        $tnids_options[$tnid] = ''; // NOTA : la valeur 'null' provoque une erreur de validation. Une chaîne vide, non.

        if(in_array($tnid, (array)$sirtaqui_content_listing->filters['nodes_filter'])) {
            $form['nodes_filter']['node_list'][$node['nid']]['#outline'] = true;
        }

        $form['nodes_filter']['node_list'][$node['nid']]['#tnid'] = $tnid;
        $form['nodes_filter']['node_list'][$node['nid']]['title'] = array('#type' => 'markup', '#value' => $node['title']);
        $form['nodes_filter']['node_list'][$node['nid']]['fnid'] = array('#type' => 'markup', '#value' => $node['fnid_value']);
        $form['nodes_filter']['node_list'][$node['nid']]['type'] = array('#type' => 'markup', '#value' => node_get_types('name', (object)$node));
        $form['nodes_filter']['node_list'][$node['nid']]['status'] = array('#type' => 'markup', '#value' => ($node['status'] ? t('published') : t('not published')));
    }

    $form['nodes_filter']['node_list']['tnids'] = array(
        '#type' => 'checkboxes',
        '#options' => $tnids_options,
        '#default_value' => (array)$sirtaqui_content_listing->filters['nodes_filter'],
        '#num_selected' => count($sirtaqui_content_listing->filters['nodes_filter']),
    );

	$form['submit'] = array('#type' => 'submit', '#value' => t('Save filters'));

	return $form;
}

/**
 * Form Submit. Edit sirtaqui content listing filters
 *
 * @ingroup forms
 * @see sirtaqui_edit_filters_sirtaqui_content_listing()
 * @see theme_sirtaqui_content_listing_filter_field()
 */
function sirtaqui_edit_filters_sirtaqui_content_listing_submit($form, &$form_state) {

	//krumo($form_state);
	$sirtaqui_content_listing = sirtaqui_load_sirtaqui_content_listing($form_state['values']['id']);
    $filters = unserialize($sirtaqui_content_listing->filters);
	$filters['filters'] = array();
	foreach ($form_state['values'] as $key => $value) {
		if (preg_match('/^content_type_/', $key)) {
			$filters['filters'][$value] = array();
		}
	}
	foreach ($form_state['values'] as $key => $value) {
		if (preg_match('/filter_fields_[0-9]+$/', $key)) {
			foreach ($filters['filters'] as $content_type => $flt) {
				if (preg_match('/'.$content_type.'/', $key))
					$filters['filters'][$content_type][$value] = $form_state['values'][$key.'_input'];

				$filters['filters'][$content_type] = array_filter($filters['filters'][$content_type]);
			}
		}
	}

    $filters['nodes_filter'] = array_values(array_filter($form_state['values']['tnids']));

	// On crée le modèle dans la base
	$sirtaqui_content_listing = array(
		'id' => $form_state['values']['id'],
		'content_listing_name' => $sirtaqui_content_listing->content_listing_name,
		'content_listing_type' => $sirtaqui_content_listing->content_listing_type,
		'description' => $sirtaqui_content_listing->description,
		'sirtaqui_content' => $sirtaqui_content_listing->sirtaqui_content,
		'filters' => serialize($filters),
		'cache' => $sirtaqui_content_listing->cache,
	);

	if (!sirtaqui_add_sirtaqui_content_listing($sirtaqui_content_listing))
	{
		drupal_set_message(t('Error while saving sirtaqui content listing '), 'error');
		$form_state['redirect'] = "admin/content/sirtaqui/content_listing/list";
		return;
	}

	return;

}

/**
 * Form Theme. Edit sirtaqui content listing filters
 *
 * @ingroup forms
 * @see sirtaqui_edit_filters_sirtaqui_content_listing()
 * @see sirtaqui_edit_filters_sirtaqui_content_listing_submit()
 */
function theme_sirtaqui_content_listing_filter_field($field) {

	$rows = array();
	foreach ($field as $key => $item) {
		if (preg_match('/^filter_[0-9]+/', $key)) {

			$row = array();
			foreach ($item as $index => $value) {
				if (preg_match('/filter_fields/', $index))
					$row[] = drupal_render($value);
			}

			$rows[] = array('data' => $row, 'class' => '');
		}
	}

	return theme('table', array(), $rows, array('class' => 'sirtaqui_content_listing_filter')).drupal_render($field['content_type']).drupal_render($field['add_filter']);
}

function sirtaqui_content_listing_update_filter_input($content_type_name, $filter) {

	// Récupération de l'ID unique du formulaire
	$form_build_id = $_POST['form_build_id'];

	// On fabrique un faux form_state
 	$form_state = array('submitted' => FALSE);

 	// Récupération du formulaire à partir du cache
	$form = form_get_cache($form_build_id, $form_state);

	// On enlève le readonly
	unset($form[$content_type_name]['filter_'.$filter][$content_type_name.'_filter_fields_'.$filter.'_input']['#attributes']['readonly']);

	$field_info = content_fields($_POST[$content_type_name.'_filter_fields_'.$filter]);
	if ($field_info['type'] == 'content_taxonomy')
		$form[$content_type_name]['filter_'.$filter][$content_type_name.'_filter_fields_'.$filter.'_input']['#autocomplete_path'] = 'taxonomy/autocomplete/'.$field_info['vid'];
	else
		$form[$content_type_name]['filter_'.$filter][$content_type_name.'_filter_fields_'.$filter.'_input']['#autocomplete_path'] = 'admin/content/sirtaqui/content_listing/edit/js/autocomplete_field/'.$content_type_name.'/'.$_POST[$content_type_name.'_filter_fields_'.$filter];

	// Sauvegarde du formulaire dans le cache
	form_set_cache($form_build_id, $form, $form_state);

	// Reconstruction du formulaire
	$form = form_builder($_POST['form_id'], $form, $form_state);

	// Récupération de notre élément reconstruit
	$element = $form[$content_type_name]['filter_'.$filter][$content_type_name.'_filter_fields_'.$filter.'_input'];

	// Transformation de l'élément en HTML
	$output = drupal_render($element);

	drupal_json(array('data' => $output, 'status' => TRUE));
	exit();

}

function sirtaqui_content_listing_autocomplete_field($content_type_name, $field, $value) {

    // la majeur partie des valeur se trouve dans les champs-f
    if (!preg_match('/field_f_/', $field)) {
    	$field = normalize_field_name(str_replace('field_', 'field_f_', $field));
    }

	$field_info = content_fields($field);
	$table = content_database_info($field_info);

	// The user enters a comma-separated list of tags. We only autocomplete the last tag.
	$array = drupal_explode_tags($value);

	// Fetch last tag
	$last_string = trim(array_pop($array));
	$matches = array();
	if ($last_string != '') {
		$sql = db_rewrite_sql("SELECT DISTINCT ".$field."_value FROM {".$table['table']."} WHERE LOWER(".$field."_value) LIKE LOWER('%$last_string%')", $table['table']);
		$result = db_query($sql);

		$prefix = count($array) ? implode(', ', $array) .', ' : '';

		while ($res = db_fetch_array($result)) {
			$n = $res[$field.'_value'];
			//$matches[$res[$field.'_value']] = $res[$field.'_value'];
			// Commas and quotes in terms are special cases, so encode 'em.
			if (strpos($res[$field.'_value'], ',') !== FALSE || strpos($res[$field.'_value'], '"') !== FALSE) {
				$n = '"'. str_replace('"', '""', $res[$field.'_value']) .'"';
			}
			$matches[$prefix . $n] = check_plain($res[$field.'_value']);
		}
	}

	drupal_json($matches);
	exit();
}

function sirtaqui_content_listing_add_content_filter($content_type_name) {

	$content_type = content_types($content_type_name);

	// Le filtre ne se fera que sur les champs non Sirtaqui
	$fields = array('' => t('-- Select a value --'));
	foreach ($content_type['fields'] as $name => $field) {
		if (preg_match('#^field_(?!f_)#', $name) && ($field['type'] == 'text' || $field['type'] == 'content_taxonomy'))
			$fields[$name] = $field['widget']['label'];
	}

	// Récupération de l'ID unique du formulaire
	$form_build_id = $_POST['form_build_id'];

	// On fabrique un faux form_state
 	$form_state = array('submitted' => FALSE);

 	// Récupération du formulaire à partir du cache
	$form = form_get_cache($form_build_id, $form_state);

	$nb_filters = count($form[$content_type_name]) - 1;

	$form[$content_type_name]['filter_'.$nb_filters] = array(
		'#type' => 'markup',
		'#title' => t(''),
		'#theme' => 'sirtaqui_content_listing_filter_field',
		'#attributes' => array('id' => $content_type_name.'_filter_0')
	);
	$form[$content_type_name]['filter_'.$nb_filters]['filter_choice'] = array(
		'#type' => 'select',
		'#title' => t(''),
		'#required' => TRUE,
		'#options' => $fields,
		'#ahah' => array(
			'path' => 'admin/content/sirtaqui/content_listing/edit/js/update_filter_input/'.$content_type_name.'/'.'filter_'.$nb_filters,
			'wrapper' => 'filter_input_'.$nb_filters.'-wrapper',
			'effect' => 'fade',
			'method' => 'replace',
		),
	);
	$form[$content_type_name]['filter_'.$nb_filters]['filter_input'] = array(
		'#type' => 'textfield',
		'#title' => t(''),
		'#required' => TRUE,
		'#options' => $fields,
		'#size' => 40,
		'#id' => 'filter_input_'.$nb_filters,
		'#attributes' => array('readonly' => 'readonly')
	);

	// Sauvegarde du formulaire dans le cache
	form_set_cache($form_build_id, $form, $form_state);

	// Reconstruction du formulaire
	$form = form_builder($_POST['form_id'], $form, $form_state);

	// Récupération de notre élément reconstruit
	$element = $form[$content_type_name]['filter_'.$nb_filters];

	// Transformation de l'élément en HTML
	$output = drupal_render($element);

	drupal_json(array('data' => $output, 'status' => TRUE));
	exit();
}


/***
 * DELETE CONTENT LISTING
 */
/**
 * Form builder for the sirtaqui content listing delete confirmation form.
 *
 */
function sirtaqui_delete_sirtaqui_content_listing($form_state, $id) {

	$sirtaqui_content_listing = sirtaqui_load_sirtaqui_content_listing($id);

	if (is_null($sirtaqui_content_listing)) {
		drupal_set_message(t('Bad argument given'), 'error');
		drupal_goto('admin/content/sirtaqui/content_listing/list');
	}

	$form['id'] = array('#type' => 'value', '#value' => $sirtaqui_content_listing->id);
	$form['label'] = array('#type' => 'value', '#value' => $sirtaqui_content_listing->content_listing_name);

	return confirm_form($form,
						t('Are you sure you want to delete the content listing %title?', array('%title' => $sirtaqui_content_listing->content_listing_name)),
						'admin/content/sirtaqui/content_listing/list',
						t('This action cannot be undone.'),
						t('Delete'),
						t('Cancel'));
}

/**
 * Submit handler to delete a sirtaqui content listing after confirmation.
 *
 * @see sirtaqui_delete_sirtaqui_content_listing()
 */
function sirtaqui_delete_sirtaqui_content_listing_submit($form, &$form_state) {

	sirtaqui_del_sirtaqui_content_listing($form_state['values']['id']);

        // on supprime le formulaire de recherche
        so_forms_destroy('sirtaqui_content_listing', $form_state['values']['id'] . '@search');

	drupal_set_message(t('Deleted sirtaqui content listing %label.', array('%label' => $form_state['values']['label'])));

	watchdog('sirtaqui_content_listing', 'Deleted sirtaqui content listing %label.', array('%label' => $form_state['values']['label']), WATCHDOG_NOTICE);

	$form_state['redirect'] = 'admin/content/sirtaqui/content_listing/list';

	return;
}