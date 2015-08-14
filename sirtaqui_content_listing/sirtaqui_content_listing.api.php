<?php

/**
 * Informs modules that a new listing has just been created.
 * 
 * @param object $content_listing : the newly created listing
 */
function hook_scl_create($content_listing) {}

/**
 * Modules own provided search field query process
 *
 * User inputs are provided "as it". Modules can choose to use sirtaqui_content_listing_prepare_query_fragment_data() or not.
 * Variables can be passed to hook_scl_query() by $compiler_options parameter.
 *
 * @param array $field
 * @param array $query
 * @param array &$compiler_options
 * [...] additional arguments
 */
function hook_scl_search($field, &$query, &$compiler_options) {}

/**
 * Modules can alter the query, just before SQL compilation
 *
 * @param array &$query
 * @param array &$compiler_options
 * @param stdClass $content_listing
 */
function hook_scl_query(&$query, &$compiler_options, $content_listing) {}