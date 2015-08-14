<?php

/**
 *
 * Theme implementation of node-[content type].tpl.php to display a node of 'listing' type
 *
 * @see node.tpl.php for common variables
 *
 * Content type specific variables :
 *      - $clid : helper. Alias for cck listing select box value.
 * 
 * @see sirtaqui_listing.tpl.php
 * @see sirtaqui_content_listing_preprocess_node()
 */
?>

<?php print($content); ?>

<?php print(so_form('sirtaqui_content_listing', $clid . '@search')); ?>

<?php print(so_form('sirtaqui_exposed_sorting', $clid)); ?>

<?php print(scl_embed_listing($clid)); ?>

<?php print(theme('pager')); ?>

