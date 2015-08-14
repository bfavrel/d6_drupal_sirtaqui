<?php

class FormWidgetSorting extends FormWidgetRadios
{
    // TODO : à implémenter
    
    public function widgetConfigurationForm(array $form_state, array $configuration, $lang, array $stored_values, $available_values) {
        
        // ce widget possède t-il un formulaire de configuration ?
        if(empty($form_state)) {return true;}
        
        $form = parent::widgetConfigurationForm($form_state, $configuration, $lang, $stored_values, $available_values);
        
        $form['options']['#title'] = t("Sorting items options");
        
        return $form;
    }
}
