<?php

class FormFieldSorting extends FormFieldText
{
    // TODO : à implémenter
    
    public function __construct(stdClass $form_field, FormWidgetAbstract $widget, $lang) {
        parent::__construct($form_field, $widget, $lang);
        
        // PROVISOIRE : Afin de bénéficier du form de config des widgets multi-values (notamment 'radios'), on duppe la classe 'FormFieldText'.
        $this->_widget_name = 'radios';
    }
    
    public static function getCompatibleWidgets(array $field_definition) {
        return array(0 => 'sorting');
    }
    
    public function fieldConfigurationForm(array $form_state) {
        $form = parent::fieldConfigurationForm($form_state);
        
        // ce widget possède t-il un formulaire de configuration ?
        if(empty($form_state)) {
            return $form;
        }

        $form['configuration']['options_mode'] = array(
            '#type' => 'value',
            '#default_value' => 1, // "erreur" volontaire : @see FormWidgetCheckboxes::themeWidgetConfigurationForm()
        );
        
        return $form;
    }
    
}
