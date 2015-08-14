
$(document).ready(function(){

// ----- SIRTAQUI FIELDS HELPERS -----    
    $('select.fields_helper').change(function(){
        var field_name = $(this).parents('fieldset').find('input.field_name_helper');
        var field_f_name = $(this).parents('fieldset').find('input.field_f_name_helper');

        if($(this).val() == 0) {
            field_name.val(null);
            field_f_name.val(null);
            return;
        }

        var values = $(this).val().split('|');

        field_name.val(values[0] + (values[3] != undefined ? values[3] : values[2]));
        field_name.focus();
        field_name.select();

        if(values[1] != '') {
            field_f_name.val(values[1] + (values[3] != undefined ? values[3] : values[2]));
        } else {
            field_f_name.val(null);
        }
    });

    $('input.field_name_helper, input.field_f_name_helper').click(function(){
        $(this).focus();
        $(this).select();
    });

// ----- MENU OVERVIEW SHOW/HIDE LINKS -----
    $('input#edit-show-disabled').click(function(event) {
        event.preventDefault();
        $('tr.menu-disabled').show();
    });

    $('input#edit-hide-disabled').click(function(event) {
        event.preventDefault();
        $('tr.menu-disabled').hide();
    });
});
