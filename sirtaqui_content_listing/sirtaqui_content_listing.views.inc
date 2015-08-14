<?php
// $Id$

/**
 * @file
 * Administrate content listing
 *
 * @author Benoit Favrel
 */

/**
* Implementation of hook_views_api().
*/
function sirtaqui_content_listing_views_api() {
  return array(
    'api' => 2.0
  );
}

/***
 * CREATE THE SIRTAQUI CONTENT LISTING VIEW
 */

function sirtaqui_generate_sirtaqui_content_listing_view($ctlid) {

	$sirtaqui_content_listing = sirtaqui_load_sirtaqui_content_listing($ctlid);
	$sirtaqui_content_listing->sirtaqui_content = unserialize($sirtaqui_content_listing->sirtaqui_content);
	$sirtaqui_content_listing->filters = unserialize($sirtaqui_content_listing->filters);
	$sirtaqui_content_listing->exposed_fields = unserialize($sirtaqui_content_listing->exposed_fields);

	$view = views_get_view('sirtaqui_' . $sirtaqui_content_listing->content_listing_type);

	if (is_null($view)) {
		$view = _create_sirtaqui_content_listing_view($sirtaqui_content_listing);
		$handler = $view->display['default']->handler;
	}
	else {
		$view->set_display('default');
		$handler = $view->display_handler;
	}
	
	// Préparation de la liste des contenus à lister
	$sirtaqui_contents_selected = array();
	$sirtaqui_contents = array();
	foreach ($sirtaqui_content_listing->sirtaqui_content as $sirtaqui_content) {
		$sirtaqui_contents[$sirtaqui_content] = $sirtaqui_content;
		$sirtaqui_contents_selected[$sirtaqui_content] = 1;
	}

	// Préparation des critères de filtrage
	$criterias = array();

	$filters = array(
	  	'status' => array(
	    	'operator' => '=',
		    'value' => '1',
		    'group' => '0',
		    'exposed' => FALSE,
		    'expose' => array(
	      		'operator' => FALSE,
	      		'label' => '',
	    	),
	    	'id' => 'status',
		    'table' => 'node',
		    'field' => 'status',
		    'relationship' => 'none',
	  	),
		'type' => array(
	   		'operator' => 'in',
                        'exposed' => FALSE,
                        'value' => $sirtaqui_contents,
                        'options' => array(
                        'value' => $sirtaqui_contents
	    	),
		    'id' => 'type',
		    'table' => 'node',
		    'field' => 'type',
		    'relationship' => 'none',
	  	),
		'language' => array(
        	'operator' => 'in',
        	'value' => array(
        		'***CURRENT_LANGUAGE***' => '***CURRENT_LANGUAGE***',
            ),
        	'group' => '0',
            'exposed' => FALSE,
            'expose' => array(
            	'operator' => FALSE,
            	'label' => '',
            ),
            'id' => 'language',
            'table' => 'node',
            'field' => 'language',
            'relationship' => 'none',
        ),
	);

	$filters_additions = array();

	if (!empty($sirtaqui_content_listing->filters[$sirtaqui_content_listing->sirtaqui_content[0]])) {
		
        foreach($sirtaqui_content_listing->filters[$sirtaqui_content_listing->sirtaqui_content[0]] as $filter_name => $filter_value) {

            $field_infos = content_fields($filter_name);

            switch($field_infos['type']) {

                case 'content_taxonomy':

                    // On récupère tous les vocabulaires liés à ce type de contenu
                    $vocabularies = taxonomy_get_vocabularies($sirtaqui_content_listing->sirtaqui_content[0]);
                    $vids = array();
                                        
                    foreach($vocabularies as $vocabulary) {
                        $vids[] = $vocabulary->vid;
                    }
                    
                    // The user enters a comma-separated list of tags. We only autocomplete the last tag.
                    $filter_values = drupal_explode_tags($filter_value);
                    
                    $term_infos = array();
                    foreach ($filter_values as $value) {
                    	$term_infos = array_merge($term_infos, taxonomy_get_term_by_name(trim($value)));
                    }
					
                    // On parcoure la liste des termes récupérés et on ne garde que les termes dont le vacabulaire est
                    // lié à ce type de contenu
                    foreach($term_infos as $key => $term_info){
                        if(!in_array($term_info->vid, $vids)) {
                        	unset($term_infos[$key]);
                        }
                    }
                    
                    //$term = taxonomy_get_term($term_info->tid);
                    $values = array();
                    foreach ($term_infos as $term_info) {
                    	$values[] = $term_info->tid;
                    }

                    $filters_additions[$filter_name] = array(
                        'operator' => 'or',
                        'value' => $values,
                        'group' => '0',
                        'exposed' => FALSE,
                        'expose' => array(
                        	'operator' => FALSE,
                        	'label' => '',
                        ),
                        'type' => 'select',
                        'limit' => TRUE,
                        'vid' => $term_info->vid,
                        'id' => 'tid',
                        'table' => 'term_node',
                        'field' => 'tid',
                        'hierarchy' => 0,
                        'relationship' => 'none',
                        'reduce_duplicates' => 0,
                    );

                    break;

                default: // type 'text'
                    $filters_additions['views_or_begin'] = array(
                        'operator' => '=',
                        'value' => '',
                        'group' => '0',
                        'exposed' => FALSE,
                        'expose' => array(
                        'operator' => FALSE,
                        'label' => '',
                        ),
                        'id' => 'views_or_begin',
                        'table' => 'views_or',
                        'field' => 'views_or_begin',
                        'relationship' => 'none',
                    );
					
                    // The user enters a comma-separated list of tags. We only autocomplete the last tag.
                    $filter_values = drupal_explode_tags($filter_value);
                    
                    foreach ($filter_values as $nb => $value) {
	                    $filters_additions[$filter_name . $nb] = array(
	                        'operator' => 'contains',
	                        'value' => trim($value),
	                        'group' => '0',
	                        'exposed' => FALSE,
	                        'expose' => array(
	                        	'operator' => FALSE,
	                        	'label' => '',
	                        	'optional' => FALSE,
	                        ),
	                        'case' => 0,
	                        'id' => $filter_name . '_value' . $nb,
	                        'table' => 'node_data_' . $filter_name,
	                        'field' => $filter_name . '_value',
	                        'relationship' => 'none',
	                    );
	                    
	                    $filters_additions['views_or_next_' . $nb] = array(
	                    		'operator' => '=',
	                    		'value' => '',
	                    		'group' => '0',
	                    		'exposed' => FALSE,
	                    		'expose' => array(
	                    				'operator' => FALSE,
	                    				'label' => '',
	                    		),
	                    		'id' => 'views_or_next_' . $nb,
	                    		'table' => 'views_or',
	                    		'field' => 'views_or_next',
	                    		'relationship' => 'none',
	                    );
                    }
                    
                    $or_oc = $nb + 1;
					
                    $filter_name = str_replace('field_f_', 'field_', $filter_name);

                    foreach ($filter_values as $nb => $value) {
	                    $filters_additions[$filter_name . ($or_oc + $nb)] = array(
	                        'operator' => 'contains',
	                        'value' => $value,
	                        'group' => '0',
	                        'exposed' => FALSE,
	                        'expose' => array(
	                        	'operator' => FALSE,
	                        	'label' => '',
	                        	'optional' => FALSE,
	                        ),
	                        'case' => 0,
	                        'id' => $filter_name . '_value' . ($or_oc + $nb),
	                        'table' => 'node_data_' . $filter_name,
	                        'field' => $filter_name . '_value',
	                        'relationship' => 'none',
	                    );
	                    
	                    $filters_additions['views_or_next_' . ($or_oc + $nb)] = array(
	                    		'operator' => '=',
	                    		'value' => '',
	                    		'group' => '0',
	                    		'exposed' => FALSE,
	                    		'expose' => array(
	                    				'operator' => FALSE,
	                    				'label' => '',
	                    		),
	                    		'id' => 'views_or_next_' . ($or_oc + $nb),
	                    		'table' => 'views_or',
	                    		'field' => 'views_or_next',
	                    		'relationship' => 'none',
	                    );
                    }
                    
                    unset($filters_additions['views_or_next_' . ($or_oc + $nb)]);

                    $filters_additions['views_or_end'] = array(
                        'operator' => '=',
                        'value' => '',
                        'group' => '0',
                        'exposed' => FALSE,
                        'expose' => array(
                        'operator' => FALSE,
                        'label' => '',
                        ),
                        'id' => 'views_or_end',
                        'table' => 'views_or',
                        'field' => 'views_or_end',
                        'relationship' => 'none',
                    );
            }

        }

	}
    
	$filters = array_merge($filters, $filters_additions);

	$handler->override_option('filters', $filters);
        
    $handler->override_option('arguments', array(
            'nid' => array(
                    'default_action' => 'empty',
                    'style_plugin' => 'default_summary',
                    'style_options' => array(),
                    'wildcard' => 'all',
                    'wildcard_substitution' => 'Tous / Toutes',
                    'title' => '',
                    'breadcrumb' => '',
                    'default_argument_type' => 'fixed',
                    'default_argument' => '',
                    'validate_type' => 'none',
                    'validate_fail' => 'not found',
                    'break_phrase' => 1,
                    'not' => 0,
                    'id' => 'nid',
                    'table' => 'node',
                    'field' => 'nid',
                    'validate_user_argument_type' => 'uid',
                    'validate_user_roles' => array(),
                    'relationship' => 'none',
                    'default_options_div_prefix' => '',
                    'default_taxonomy_tid_term_page' => 0,
                    'default_taxonomy_tid_node' => 0,
                    'default_taxonomy_tid_limit' => 0,
                    'default_taxonomy_tid_vids' => array(),
                    'default_argument_user' => 0,
                    'default_argument_fixed' => '',
                    'default_argument_php' => '',
                    'validate_argument_node_type' => array(),
                    'validate_argument_node_access' => 0,
                    'validate_argument_nid_type' => 'nid',
                    'validate_argument_vocabulary' => array(),
                    'validate_argument_type' => 'tid',
                    'validate_argument_transform' => 0,
                    'validate_user_restrict_roles' => 0,
                    'validate_argument_php' => '',
            ),
    ));

	$view->save();

	drupal_set_message(t('Views was generated for %content', array('%content' => $sirtaqui_content_listing->content_listing_name)), 'status');
	drupal_goto("admin/content/sirtaqui/content_listing/list");
        return;
}

/**
 *
 * If no view has already been created, a new one with page display is created
 */
function _create_sirtaqui_content_listing_view($sirtaqui_content_listing) {

	//Create a basic view object.
	$view = views_new_view();
	$view->name = 'sirtaqui_' . $sirtaqui_content_listing->content_listing_type;
	$view->description = $sirtaqui_content_listing->description;
	$view->tag = '';
	$view->view_php = '';
	$view->base_table = 'node';
	$view->is_cacheable = TRUE;
	$view->api_version = 2;
	$view->disabled = FALSE;

	$handler = $view->new_display('default', $sirtaqui_content_listing->content_listing_name, 'default');

	$handler->override_option('title', $sirtaqui_content_listing->content_listing_name);
	$handler->override_option('items_per_page', 8);
	$handler->override_option('row_plugin', 'node');
	$handler->override_option('row_options', array('build_mode' => 'teaser', 'links' => 0));
	$handler->override_option('use_pager', 1);
	$handler->override_option('use_ajax', 1);
        $handler->override_option('distinct', 1);
        $handler->override_option('empty', 'Aucun résultat');

	$view->save();

	return $view;
}