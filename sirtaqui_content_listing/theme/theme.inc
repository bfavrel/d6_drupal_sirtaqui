<?php


function sirtaqui_content_listing_preprocess_node(&$vars) {

    if($vars['node']->type == 'listing') {

        $vars['clid'] = $vars['field_content_listing'][0]['value'];
        
    	if (empty($vars['num_results'])) {
	        global $pager_total_items;
	    	$vars['num_results'] = $pager_total_items[0];            
    	}
    }    
}

/**
 * Preprocess function for sirtaqui_listing.tpl.php
 * @param array $vars
 */
function sirtaqui_content_listing_preprocess_sirtaqui_listing(&$vars) {
    global $pager_total_items;
    $vars['num_results'] = $pager_total_items[0];    
}
