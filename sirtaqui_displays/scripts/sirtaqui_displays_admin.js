$(document).ready(function(){

    function resetHelper(helper) {
        helper.val('0');
    }

    $('select#label_helper').change(function(){

        switch($(this).val()) {
            case '0': // nothing
                break;

            case '1': // all above
                $('td.label_helper').find("select:has(option[value='above'])").val('above');
                break;

            case '2': // all inline
                $('td.label_helper').find("select:has(option[value='inline'])").val('inline');
                break;

            case '3': // all hidden
                $('td.label_helper').find("select:has(option[value='hidden'])").val('hidden');
                break;

            default:                
                $('td.label_helper_' + $('option:selected', this).parent('optgroup').attr('label')).find('select').val($(this).val());
        }
        
        resetHelper($(this));
    });
    
    $('select.display_helper').change(function(){
        
        var contextClass = $(this).attr('id');
        
        switch($(this).val()) {
            case '0': // nothing
                break;
                
            case '1': // all default
                $('td.' + contextClass).find("select:has(option[value='default'])").val('default');
                break;
                
            case '2': // all hidden
                $('td.' + contextClass).find("select:has(option[value='hidden'])").val('hidden');
                break;
                
            default:
                $('td.' + contextClass + '_' + $('option:selected', this).parent('optgroup').attr('label')).find('select').val($(this).val());
        }
        
        resetHelper($(this));
    });
    
    $('select.exclude_helper').change(function(){
        
        var contextClass = $(this).attr('id');
        
        switch($(this).val()) {
            case '0': // nothing
                break;
                
            case '1': // exclude all
                $('td.' + contextClass).find('input.form-checkbox').attr('checked', true);
                break;
                
            case '2': // include all
                $('td.' + contextClass).find('input.form-checkbox').attr('checked', false);
                break;
                
            default:
                $('td.' + contextClass + '_' + $('option:selected', this).parent('optgroup').attr('label')).find('input.form-checkbox').attr('checked', eval($(this).val()));
        }
        
        resetHelper($(this));
    });

    function showDisplaySelectorElement(evt) {
        evt.preventDefault();

        var selectorElement = $('div#sirtaqui_displays_selector_element');

        if(selectorElement.is(':visible') == true) {return;}

        $(evt.currentTarget).unbind('click');
        $(evt.currentTarget).append(selectorElement);
        $(evt.currentTarget).css('z-index', 1);
        $(evt.currentTarget).bind('mouseleave', function(evt){
            hideDisplaySelectorElement(evt.currentTarget);
            $(evt.currentTarget).bind('click', showDisplaySelectorElement);
        });

        var selectorValue = $(evt.currentTarget).children('input:hidden').val();
        selectorValue = selectorValue != '' ? selectorValue : 'sirtaqui';
        selectorElement.find('input:radio').attr('checked', false);
        selectorElement.find('input:radio[value=' + selectorValue + ']').attr('checked', true);

        selectorElement.fadeIn('slow');
        selectorElement.bind('change', proceedWithSelectedDisplayMode);

    }

    function hideDisplaySelectorElement(pictoLink) {
        $(pictoLink).unbind('mouseleave');

        $('div#sirtaqui_displays_selector_element').fadeOut(300, function(){
            $(pictoLink).css('z-index', 0);
        });
    }

    function proceedWithSelectedDisplayMode(evt) {
        var pictoLink = $(evt.currentTarget).parents('span.sirtaqui_displays_picto_link');

        $(evt.currentTarget).unbind('change', proceedWithSelectedDisplayMode);
        hideDisplaySelectorElement(pictoLink);

        pictoLink.addClass('processing');

        var nid = pictoLink.attr('id').replace('sirtaqui_displays_picto_link_', '');
        var display = $(evt.currentTarget).find('input:radio[checked=true]').val();
        var pictoSrc = $(evt.currentTarget).find('input:radio[checked=true]').siblings('img').attr('src');

        $.ajax({
           type: 'POST',
           dataType: 'text',
           url: '/admin/content/sirtaqui/mode_displays_js/set_node_display/' + nid + '/' + display,
           success: function(data){
               pictoLink.children('input:hidden').val(display);
               pictoLink.children('img').attr('src', pictoSrc);
               pictoLink.removeClass('processing');
               pictoLink.bind('click', showDisplaySelectorElement);
           }
        });
    }

    $('span.sirtaqui_displays_picto_link').bind('click', showDisplaySelectorElement);

});