<?php
/*
 * Theme implementation to display a SIRTAqui listing.
 * 
 * Variables : 
 *      - $nodes : array of nodes objects fully loaded but not rendered yet.
 *      - $num_results : total number of results
 *  
 * @see sirtaqui_content_listing_preprocess_sirtaqui_listing()
 */
?>


<div id="total-items">
    <?php
        if($num_results > 0) {
            print(format_plural($num_results, '<span class="nb-items">1</span> result', '<span class="nb-items">@count</span> results'));  
        } else {
            print(t("No result"));
        }
    ?>
</div>

<div>
    <?php
        foreach($nodes as $node) {
            // L'argument 'teaser' est à 'true' seulement pour limiter le volume des pages des listings qui n'ont pas encore été thémés.
            print(node_view($node, true, false, false));
        }
    ?>
</div>