<?php

function sirtaqui_displays_scl_settings($form_state) {
    $form = array();
    
    $form['sirtaqui_displays_query_orderby'] = array(
        '#type' => 'radios',
        '#title' => "hook_scl_query() ORDER BY clause behavior",
        '#description' => t("This hook is called at the very end of the process, just before SQL compilation"),
        '#default_value' => variable_get('sirtaqui_displays_query_orderby', 0),
        '#options' => array(
            0 => t("None"),
            1 => t("Prepend"),
            2 => t("Append"),
            3 => t("Replace"),
        ),
    );
    
    return system_settings_form($form);
}

/***
 * LISTE, CREE, SUPPRIME AFFICHAGES DE NODE
*/

/**
 * Form builder to list and manage node dipslays.
 *
 * @ingroup forms
 * @see sirtaqui_displays_node_displays_submit()
 * @see theme_sirtaqui_displays_node_displays()
 */
function sirtaqui_displays_node_displays($form_state) {

    $displays = variable_get('sirtaqui_displays_node_displays', null);

    $form = array();

    if ($displays != null) {
        
        $destination = drupal_get_destination();

        foreach($displays as $type => $value)
        {
            $form[$type]['name'] = array('#value' => check_plain($value['title']));
            $form[$type]['type'] = array('#value' => check_plain($type));
            $form[$type]['weight'] = array('#value' => $value['weight']);
            $form[$type]['picto'] = array('#value' => !empty($value['picto']) ? theme('image', $value['picto']) : theme('image', drupal_get_path('module', 'sirtaqui_displays') . '/pictos/_sirtaqui_default.png'));
            $form[$type]['edit'] = array('#value' => l(t('edit'), "admin/content/sirtaqui/sirtaqui_displays/node_displays/edit/$type", array('query' => $destination)));
            $form[$type]['delete'] = array('#value' => l(t('delete'), "admin/content/sirtaqui/sirtaqui_displays/node_displays/delete/$type", array('query' => $destination)));
        }
    }

    $form['new_display_name'] = array(
        '#type' => 'textfield',
        '#title' => '',
        '#required' => TRUE,
        '#size' => 30,
    );
    $form['new_display_type'] = array(
        '#type' => 'textfield',
        '#title' => '',
        '#required' => TRUE,
        '#size' => 30,
    );
    $form['new_display_weight'] = array(
        '#type' => 'weight',
        '#title' => '',
        '#required' => TRUE,
    );

    $form['submit'] = array('#type' => 'submit', '#value' => t('Add'));

    return $form;
}

/**
 * Submit handler for node displays overview. Add a new node display
 *
 * @see sirtaqui_displays_node_displays()
 * @see theme_sirtaqui_displays_node_displays()
 */
function sirtaqui_displays_node_displays_submit($form, &$form_state) {

    $displays = variable_get('sirtaqui_displays_node_displays', array());

    // TODO : un peu de sécurité utilisateur - test des noms et des types existants

    $displays = array_merge($displays, array(
        $form_state['values']['new_display_type'] => array(
            'title' => $form_state['values']['new_display_name'],
            'weight' => $form_state['values']['new_display_weight'],
        )
    ));

    $displays = _sirtaqui_displays_order_by_weight($displays);
    
    variable_set('sirtaqui_displays_node_displays', $displays);

    menu_rebuild();
}
 /**
  * Helper : order displays by the weight set (saving ressources for scl sorting)
  * @param array $displays 
  */
function _sirtaqui_displays_order_by_weight($displays) {
    
    $weights = array();
    foreach ($displays as $key => $display) {
        $weights[$key] = $display['weight'];
    }
    array_multisort($weights, SORT_ASC, $displays);
    
    return $displays;
}

function theme_sirtaqui_displays_node_displays($form) {

    $rows = array();

    foreach (element_children($form) as $key) {
        if (isset($form[$key]['name']))
        {
            $theme = &$form[$key];

            $row = array();

            $row[] = drupal_render($theme['name']);
            $row[] = drupal_render($theme['type']);
            $row[] = drupal_render($theme['weight']);
            $row[] = drupal_render($theme['picto']);
            $row[] = drupal_render($theme['edit']);
            $row[] = drupal_render($theme['delete']);

            $rows[] = array('data' => $row, 'class' => '');
        }
    }

    // Add the new display form
    $row = array();
    $row[] = drupal_render($form['new_display_name']);
    $row[] = drupal_render($form['new_display_type']);
    $row[] = drupal_render($form['new_display_weight']);
    $row[] = array('colspan' => 2);
    $row[] = drupal_render($form['submit']);

    $rows[] = array('data' => $row, 'class' => 'new_display');

    $header = array(t('Readable name'), t('Internal name'), t('Weight'), t('Picto'));
    $header[] = array('data' => t('Actions'), 'colspan' => '2');

    return theme('table', $header, $rows, array('id' => 'node_display')) . drupal_render($form);
}

/**
 * Form builder for the display edition form.
 *
 * @ingroup forms
 * @see sirtaqui_displays_node_displays_edit_submit()
 */
function sirtaqui_displays_node_displays_edit($form_state, $type) {

    $displays = variable_get('sirtaqui_displays_node_displays', null);

    $form['display_name'] = array(
        '#type' => 'textfield',
        '#title' => t('Readable name'),
        '#required' => TRUE,
        '#default_value' => $displays[$type]['title'],
        '#size' => 30,
    );
    
    $form['display_weight'] = array(
        '#type' => 'weight',
        '#title' => t('Weight in listings'),
        '#required' => true,
        '#default_value' => $displays[$type]['weight'],
    );
    
    $form['display_type'] = array(
        '#type' => 'value',
        '#value' => $type,
    );

    $pictos = file_scan_directory(drupal_get_path('module', 'sirtaqui_displays') . '/pictos', '.png', array('.', '..', 'CVS'), null, false, $key = 'filename');

    array_walk($pictos, function(&$value, $key){
        $value = theme('image', $key);
    });

    $form['display_picto'] = array(
        '#type' => 'radios',
        '#title' => t("Picto"),
        '#required' => true,
        '#options' => $pictos,
        '#default_value' => $displays[$type]['picto'],
    );

    $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));

    return $form;
}

function sirtaqui_displays_node_displays_edit_submit($form, &$form_state) {

	$displays = variable_get('sirtaqui_displays_node_displays', null);

	$displays[$form_state['values']['display_type']]['title'] = $form_state['values']['display_name'];
        $displays[$form_state['values']['display_type']]['weight'] = $form_state['values']['display_weight'];
        $displays[$form_state['values']['display_type']]['picto'] = $form_state['values']['display_picto'];

        $displays = _sirtaqui_displays_order_by_weight($displays);
        
	variable_set('sirtaqui_displays_node_displays', $displays);
        
        // TODO : modifier directement le 'title' dans le routeur. Le rebuild est un peu lourd.
        menu_rebuild();

	drupal_set_message(t("The display '%name' has been updated", array('%name' => $form_state['values']['display_name'])), 'status');
	$form_state['redirect'] = 'admin/content/sirtaqui/mode_displays';
}

/**
 * Form builder for the display delete confirmation form.
 *
 * @ingroup forms
 * @see sirtaqui_displays_node_displays_confirm_delete_submit()
 */
function sirtaqui_displays_node_displays_confirm_delete($form_state, $type) {

	$displays = variable_get('sirtaqui_displays_node_displays', array());

	$form['type'] = array('#type' => 'value', '#value' => $type);
	$form['name'] = array('#type' => 'value', '#value' => $displays[$type]['title']);

	return confirm_form($form,
			t("Are you sure you want to delete the display '%title' ?", array('%title' => $displays[$type]['title'])),
			$_GET['destination'],
			t('This action cannot be undone.'),
			t('Delete'),
			t('Cancel'));
}

/**
 * Submit handler to delete a display after confirmation.
 *
 * @see sirtaqui_displays_node_displays_confirm_delete()
 */
function sirtaqui_displays_node_displays_confirm_delete_submit($form, &$form_state) {

	$displays = variable_get('sirtaqui_displays_node_displays', null);
	unset($displays[$form_state['values']['type']]);
	variable_set('sirtaqui_displays_node_displays', $displays);

	drupal_set_message(t("Deleted display '%label'.", array('%label' => $form_state['values']['name'])));
	watchdog('sirtaqui_displays', "Deleted display '%label'.", array('%label' => $form_state['values']['name']), WATCHDOG_NOTICE);

	$form_state['redirect'] = 'admin/content/sirtaqui/mode_displays';

        menu_rebuild();

	return;
}

/*
 * Duplicates Basic:teaser/full build modes settings into sirtaqui:listing/fiche
 */
function sirtaqui_displays_init_default($form_state) {
    
    return confirm_form(
                null,
                t("Are you sure you want to copy Basic:teaser/full settings into sirtaqui:listing/fiche settings for all Sirtaqui content types ?"),
                'admin/content/sirtaqui/mode_displays/list',
                t("All Sirtaqui (default) display settings will be lost.") . "<br />" . t("This action cannot be undone."),
                t("Continue"),
                t("Cancel")
            );
}

/*
 * Submit function for sirtaqui_displays_init_default()
 */
function sirtaqui_displays_init_default_submit($form, &$form_state) {

    $req = "SELECT field_name, type_name, display_settings FROM {content_node_field_instance} WHERE type_name LIKE 'sirtaqui_%%'";
    $result = db_query($req);

    while($row = db_fetch_array($result)) {
        $display_settings = unserialize($row['display_settings']);
        $display_settings['sirtaqui_listing'] = $display_settings['teaser'];
        $display_settings['sirtaqui_fiche'] = $display_settings['full'];

        $sub_req = "UPDATE {content_node_field_instance} SET display_settings='%s' WHERE field_name='%s' AND type_name='%s'";
        db_query($sub_req, serialize($display_settings), $row['field_name'], $row['type_name']);
    }

    // les forms 'Afficher les champs' sont cachés : table {cache_content}->content_type_info:*
    cache_clear_all('content_type_info:', 'cache_content', true);

    drupal_set_message(t("Sirtaqui default build mode reseted."));
    $form_state['redirect'] = 'admin/content/sirtaqui/mode_displays/list';
}

/**
 * JS Callback for setting one node display mode from Drupal content listing form
 * @param int $nid
 * @param string $display 
 */
function sirtaqui_displays_js_set_node_display($nid, $display) {
    
    $req = "REPLACE INTO {sirtaqui_displays} SET nid=%d, display='%s'";
    db_query($req, $nid, $display);
    
    return drupal_json();
}
