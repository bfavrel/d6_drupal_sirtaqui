<?php

/**
 * Provides settings menu items to sirtaqui module.
 * All these settings are gathered as local tasks of their module under 'admin > settings > SIRTAqui' menu item.
 * 
 * Syntax is the same as usual menu items syntax.
 * Use only keys listed below. No 'type', no 'access callback', etc.
 * 
 * @return array of arrays indexed by a string which identify the item in the context of its module ; Choose it wisely : this key is used in menu path.
 *          - 'title'
 *          - 'page callback'
 *          - 'page arguments'
 *          - 'file'
 *          - 'file path' : optional : if needed but empty, module path will be assumed.
 *                  
 */
function hook_sirtaqui_settings() {    
    return array(
        'list' => array(
            'title' => 'List',
            'page callback' => 'drupal_get_form',
            'page arguments' => array('sirtaqui_displays_node_displays'),
            'file' => 'sirtaqui_displays.admin.inc',
            'file path' => drupal_get_path('module', 'sirtaqui_displays') . '/my_include_directory', // required 
        ),
        'init' => array(
            'title' => 'Init default build mode',
            'page callback' => 'drupal_get_form',
            'page arguments' => array('sirtaqui_displays_init_default'),
            'file' => 'sirtaqui_displays.admin.inc',
            // 'file path' : not required here since script stands at module's root
        ),
    );
}