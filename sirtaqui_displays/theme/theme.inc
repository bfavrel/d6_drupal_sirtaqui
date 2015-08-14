<?php

function sirtaqui_displays_preprocess_sirtaqui_listing(&$vars) {
    foreach($vars['nodes'] as &$node) {
        // NOTE : ce BM est pris en compte par CCK dans content.module:730
        $node->build_mode = sirtaqui_displays_get_display_from_nid($node->nid) . '_listing';
    }
}

function sirtaqui_displays_preprocess_content_display_overview_form(&$vars) {

    $contexts = array_keys($vars['contexts']);
    $indices = _content_overview_order($vars['form'], $vars['form']['#fields'], $vars['form']['#groups']);
    $label_options = array();
    $bm_options = array();
    $exclude_options = array();

    foreach($indices as $index => $field_name) {
        if(!isset($vars['form'][$field_name][$contexts[0]])) {continue;}
        
        // on ignore les champs_f : leur paramètres ne sont pas pris en compte pour l'affichage.
        if(preg_match('#^field_f_#', $field_name) != false) {continue;}

        $field_infos = content_fields($field_name, $vars['form']['#type_name']);

        $options_type = $field_infos != null ? $field_infos['type'] : t("group");

        if($vars['basic'] == true) {
            $label_options[$options_type] = (array)$vars['form'][$field_name]['label']['format']['#options'] + (array)$vars['form'][$field_name]['label']['#options'];
            $vars['rows'][$index]->label_helper_class = 'label_helper_' . $options_type;
        }

        $bm_options[$options_type] = (array)$vars['form'][$field_name][$contexts[0]]['format']['#options'] + (array)$vars['form'][$field_name][$contexts[0]]['format']['base']['#options'];
        $exclude_options[$options_type] = array('true' => t("Exclude"), 'false' => t("Include"));

        foreach($contexts as $context) {
            $vars['rows'][$index]->display_helper_class = 'display_helper_' . $options_type;
            $vars['rows'][$index]->exclude_helper_class = 'exclude_helper_' . $options_type;
        }
    }

    if($vars['basic'] == true) {
        $label_options = array_merge(array(
            0 => "- " . t("settings") . " -",
            1 => t("All above"),
            2 => t("All inline"),
            3 => t("All hidden"),
        ), $label_options);

        $vars['helpers']['label_helper'] = array(
            '#type' => 'select',
            '#id' => 'label_helper',
            '#title' => '',
            '#default_value' => 0,
            '#options' => $label_options,
            '#attributes' => array('style' => 'max-width: 137px;'),
        );
    }

    $order = array();
    foreach ($bm_options as $options_type => $options) {
        $order[$options_type] = count($options);
    }
    array_multisort($order, SORT_ASC, $bm_options);

    $bm_options = array_merge(array(
        0 => "- " . t("settings") . " -",
        1 => t("All default"),
        2 => t("All hidden"),
    ), $bm_options);
    
    $exclude_options = array_merge(array(
        0 => "- " . t("settings") . " -",
        1 => t("Exclude all"),
        2 => t("Include all"),
    ), $exclude_options);

    foreach($contexts as $context) {
        $vars['helpers'][$context . '_display_helper'] = array(
            '#type' => 'select',
            '#id' => $context . '_display_helper',
            '#title' => '',
            '#default_value' => 0,
            '#options' => $bm_options,
            '#attributes' => array('class' => 'display_helper', 'style' => 'max-width: 209px;'),
        );
        
        $vars['helpers'][$context . '_exclude_helper'] = array(
            '#type' => 'select',
            '#id' => $context . '_exclude_helper',
            '#title' => '',
            '#default_value' => 0,
            '#options' => $exclude_options,
            '#attributes' => array('class' => 'exclude_helper', 'style' => 'max-width: 120px;'),
        );
    }
}
