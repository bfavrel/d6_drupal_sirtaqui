<?php
// $Id$

/**
 * @file
 * Content creation from imported infos
 */

/**
 * Update or create a content from information and a list of the content fields
 *
 * @param stdClass $sirtaqui_content : sirtaqui content set in database
 * @param array $infos : informations from webservice => array( $field_name => $value, ...)
 * @param array $field_list : list of field from the content to create => array( $field_name1, $field_name2, ...)
 */
function update_content_from_infos($sirtaqui_content, $infos, $field_list = array(), $force_update = FALSE) {

    $logs = variable_get('fiche_logs', array());
    
    if($logs['next'] == true) {
        watchdog('sirtaqui_fiche', "Next fiche : @fnid (@language)", array(
                '@fnid' => $infos['ID'],
                '@language' => $sirtaqui_content->language,
            ), WATCHDOG_INFO);
    }

    $import_filters = array_filter((array)$sirtaqui_content->import_filters);

    // Première passe : on ne prend en compte que les éventuels échecs qualifiés.
    if(_sirtaqui_import_xml_filtering($import_filters, $infos) === false) {
        
        if($logs['filter'] == true) {
            watchdog('sirtaqui_fiche', "Filtered 1 : @fnid (@language)", array(
                '@fnid' => $infos['ID'],
                '@language' => $sirtaqui_content->language,
            ), WATCHDOG_INFO);
        }
        
        return;
    }

	$default_language = language_default();
    $default_language = $default_language->language;

    $fnid_table = content_database_info(content_fields('fnid')); // s'il n'y a qu'un seul type, il n'y a pas de table distincte

    // on récupère les deux nodes associés au FNID : celui en langue par défaut ainsi que celui dans la langue du bordereau
    $req = "SELECT n.nid FROM {" . $fnid_table['table'] . "} AS fnid
            LEFT JOIN {node} AS n ON n.nid=fnid.nid
            WHERE fnid.fnid_value='%s'
            AND (n.language='%s'
            OR n.language='%s')
            ORDER BY n.nid DESC"; // le premier enregistrement sera donc celui que l'on souhaite updater

    $result = db_query($req, $infos['ID'], $default_language, $sirtaqui_content->language);

    $node = db_fetch_object($result);

    // soit aucun node n'a été trouvé et nous sommes en langage par défaut,
    // soit un seul node est trouvé (celui en langage par défaut) : on crée un node en langue étrangère en se basant sur les infos de ce node.
    // ATTENTION : la variable num_rows n'est dispo que pour MySQL/MySQLi
    if(($result->num_rows == 0 && $sirtaqui_content->language == $default_language) || ($result->num_rows == 1 && $sirtaqui_content->language != $default_language)) {

        $content = new stdClass();

        $content->tnid = $node->nid;

        if($sirtaqui_content->language != $default_language) {
            db_query("UPDATE {node} SET tnid=nid WHERE nid=%s", $node->nid);
        }

        $content->uid = 1;
		$content->language = $sirtaqui_content->language;
		$content->type = 'sirtaqui_' . $sirtaqui_content->content_type;
        $content->status = 1;

        if(!is_null($infos['DATECREA']))
		{
			$date = DateTime::createFromFormat('Y-m-d\TH:i:sP', $infos['DATECREA']);
			$content->created = $date->getTimestamp();
		}

        $content->date_maj[0] = array('value' => 0);

        $content->fnid[0] = array('value' => $infos['ID']);
		$content->taxonomy = array();

    // c'est une mise à jour de node existant
    } elseif(!empty($node)) {

        $datemaj_table = content_database_info(content_fields('date_maj')); // s'il n'y a qu'un seul type, il n'y a pas de table distincte
        $last_datemaj = db_result(db_query("SELECT date_maj_value FROM {" . $datemaj_table['table'] . "} WHERE nid=%d", $node->nid));

        $content->nid = $node->nid;
        $content->date_maj[0]['value'] = (int)$last_datemaj;

    // on se trouve dans le cas d'un node en langue étrangère, qui n'a pas de node de référence en langue par défaut
    // (ce cas peut survenir, si le node en language par défaut a été écarté par le filtre d'import, et que les champs de filtrage
    // ne se trouvent que dans le getDetail() )
    } else {
        if($logs['abortion'] == true) {
            watchdog('sirtaqui_fiche', "Aborted : @fnid (@language)", array(
                    '@fnid' => $infos['ID'],
                    '@language' => $sirtaqui_content->language,
                ), WATCHDOG_INFO);
        }
        
        return;
    }

    if (!is_null($infos['DATEMAJ']))
    {
        $sirtaqui_datemaj = DateTime::createFromFormat('Y-m-d\TH:i:sP', $infos['DATEMAJ']);
        $sirtaqui_datemaj = $sirtaqui_datemaj->getTimestamp();
    } else {
        $sirtaqui_datemaj = time(); // dans le doute, on met quand-même à jour                
        watchdog('sirtaqui_import', "DATEMAJ is null for fiche : @fnid", array('@fnid' => $infos['ID']), WATCHDOG_WARNING);
    }

    if($content->date_maj[0]['value'] >= $sirtaqui_datemaj && $force_update != true) {

        if($logs['up_to_date'] == true) {
            watchdog('sirtaqui_fiche', "Up to date : @fnid (@language)", array(
                '@fnid' => $infos['ID'],
                '@language' => $sirtaqui_content->language,
            ), WATCHDOG_INFO);
        }
        
        if($sirtaqui_content->params['actions']['unpublished_node'] == 'publish') {
            db_query("UPDATE {node} SET status=1 WHERE nid=%d", $content->nid);
        }

        return $content->nid;
    }

    $content->date_maj[0]['value'] = $sirtaqui_datemaj;

    $infos = _sirtaqui_get_content_details($sirtaqui_content->module_id, $infos);

    // Seconde passe : cette fois le filtrage est strict
    if(_sirtaqui_import_xml_filtering($import_filters, $infos) != true) {
        
        if($logs['filter'] == true) {
            watchdog('sirtaqui_fiche', "Filtered 2 : @fnid (@language)", array(
                '@fnid' => $infos['ID'],
                '@language' => $sirtaqui_content->language,
            ), WATCHDOG_INFO);
        }
        
        return;
    }

    $operation = empty($content->nid) ? t("Creation@fiche", array('@fiche' => null)) : t("Update@fiche", array('@fiche' => null));

    if($logs['start'] == true) {
        watchdog('sirtaqui_fiche', $operation . " start : @fnid (@language)", array(
                '@fnid' => $infos['ID'],
                '@language' => $sirtaqui_content->language,
            ), WATCHDOG_INFO);
    }

    $node = node_load($content->nid);

    if(!empty($node)) {
        $content = (object)array_merge((array)$node, (array)$content);
    }

    _update_content(&$content, $infos, $field_list, $sirtaqui_content);

    if($sirtaqui_content->params['actions']['unpublished_node'] == 'publish') {
        $content->status = 1;
    }

    if(empty($content->title) || $sirtaqui_content->params['title_update_restriction'] != true) {

        $title_fields = array_filter((array)$sirtaqui_content->params['title_fields']);

        $title_fields = empty($title_fields) ? array($sirtaqui_content->title_field => $sirtaqui_content->title_field) : $title_fields;

        foreach($title_fields as $field => $value) {
            // on part du principe qu'on a eu le bon sens de choisir des champs non susceptibles de contenir des images, des URI,
            // des n° de téléphone...donc, on ne passe pas par les filtres d'import. Toutefois, avec le SIRTAqui, on n'est jamais à
            // l'abri d'un séparateur dans un champ 'raison sociale'...
            if(empty($infos[$field])) {continue;}

            $field_value = explode(variable_get('sirtaqui_separator', '|'), $infos[$field]);

            $content->title = $field_value[0];

            if(!empty($content->title)) {break;}
        }
    }

    if(empty($content->title)) {

        watchdog('sirtaqui_import', "Title missing for fiche : @fnid<br />Node : @nid<br />Content : @content<br />Language : @language", array(
            '@fnid' => $infos['ID'],
            '@nid' => !empty($content->nid) ? $content->nid : t("Node not created yet"),
            '@content' => $sirtaqui_content->content_name . " (id : " . $sirtaqui_content->id . ")",
            '@language' => $sirtaqui_content->language,
        ), WATCHDOG_WARNING);

        $content->status = 0;
    }

    // On retire à filefield la responsabilité de la gestion des anciens fichiers
    // @see filefield_field.inc:186:filefield_field_update()
    $content->skip_filefield_delete = true;

	node_save($content);

    if($logs['end'] == true) {
        watchdog('sirtaqui_fiche', $operation . " over : @fnid (@language)", array(
                '@fnid' => $infos['ID'],
                '@language' => $sirtaqui_content->language,
            ), WATCHDOG_INFO);
    }

	return $content->nid;
}

/**
 * Pass given values through filters.
 * Return false only if value exists and doesn't match the filter.
 * If a value pass the filter, it is removed from the filters array.
 *
 * @param array &$filters
 * @param array $infos
 *
 * @return boolean or null : 'true' = filter passed, 'false' = filter failed, 'null' = missing at least one value to test
 */
function _sirtaqui_import_xml_filtering(&$filters, $infos) {
    foreach($filters as $item => $value) {

        // isset() ne détecte pas les valeurs 'null' ; un second check (plus lourd) est nécessaire, dans ce cas.
        if((isset($infos[$item]) || array_key_exists($item, $infos))) {

        	// Si les valeurs sont séparées par une virgule, on fait un où sur toutes ces valeurs
        	$values = explode(',', $value);
        	
        	// transtypage (string) : au second passage les valeurs nulles sont des array vides (???)
        	$sirtaqui_values = explode(variable_get('sirtaqui_separator', '|'), (string)$infos[$item]);
        	
        	if (count($values) > 1) {
        		$found = false;
        		foreach ($values as $or_value) {
	        		if (in_array($or_value, $sirtaqui_values) == true) {
	        			// filtre passé : pas besoin de refiltrer sur cette valeur ultérieurement
	        			unset($filters[$item]);
	        			$found = true;
	        			break;
	        		}
        		}
        		
        		if (!$found) {
        			return false;
        		}
        	}
        	else {
	            if(in_array($value, $sirtaqui_values) == true) {
	                // filtre passé : pas besoin de refiltrer sur cette valeur ultérieurement
	                unset($filters[$item]);
	            } else {
	                return false;
	            }
        	}
        	
        } else {
            return null; // si une valeur est manquante, inutile de continuer
        }
    }

    return empty($filters) ? true : null;
}

function _update_content(&$content, $infos, $field_list, $sirtaqui_content) {

	foreach ($infos as $key => $value) {

		if (!empty($value) && !is_array($value)) {

           	// Si le champ-f a été ajouté manuellement alors qu'existait déjà un champ-non-f correspondant, et vide,
           	// la mécanique d'override des valeurs (@see sirtaqui.module:nodeapi()) ne peut pas fonctionner. En effet,
           	// le champ non-f n'existe pas dans le cache CCK (@see {cache_content} 'content:nid:vid') et est donc ignoré
           	// par le node_[load/view]() du node.
           	// Il est donc nécessaire de contrôler, et, le cas échéant, de peupler ce champ avec une valeur nulle afin de
           	// lui donner une existence au yeux de CCK.
           	$field_no_f = normalize_field_name('field_' . $key);
           	if(!isset($content->$field_no_f)) {
            	$content->$field_no_f = array();
           	}

			if (function_exists($content->type . '_filter_' . $key)) {
				call_user_func($content->type . '_filter_' . $key, $value, $content);
			}
			elseif(function_exists('sirtaqui_filter_' . $key)) {
				call_user_func('sirtaqui_filter_' . $key, $value, $content);
			}
			else {
				switch ($key)
				{
					case 'ID':
					case 'DATECREA' :
					case 'DATEMAJ' :
					case 'STRUCTURE_ID':
					case 'OBJETTOUR_CODE' :
					case 'OBJETTOUR_LIBELLE' :
					case 'SORT_SPECIFIQUE' :
					case 'NOTPANIER' :
						break;
					default :
						$key = normalize_field_name('field_f_'.$key);

						// Cherche si le champs est présent dans la liste des champs du contenu
						if (array_key_exists($key, $field_list)) {

							// Si plusieurs valeurs alors elles sont séparées par un $
							// $values devient obligatoirement un tableau

							$values = explode(variable_get('sirtaqui_separator', '|'), trim($value));
							array_walk($values, function(&$val){
								$val = trim($val);
							});

							// Supprime les entrèes vides du tableau
							if (!empty($values))
								$values = array_filter($values, "_empty_table_value");

							if (empty($values))
								continue;

							//Si il n'y a qu'une entrée on supprime le tableau et garde que la valeure
							//if (is_array($values) && count($values) == 1)
								//$values = (string) $values[0];

							// Il est possible qu'il manque des séparations pour les URL d'image : traitement ici de ces cas
							if ($field_list[$key]['type'] == 'filefield') {
									$new_values = array();
									foreach ($values as $val) {
										$res = _field_filter_media($val);

										if (is_array($res))
											$new_values = array_merge($new_values, $res);
										else
											$new_values[] = $res;
									}
									$values = $new_values;
							}

							// On filtre les différents contenu nécessaires (photos...)
							foreach ($values as $values_key => $val)
							{
								if ($field_list[$key]['type'] == 'filefield')
								{
									// Si c'est une image renvoie array('filepath' => url) sinon array('value' => value)
									$values[$values_key] = _field_filter_picture($val);
								}
								elseif ($field_list[$key]['type'] == 'content_taxonomy')
								{
									$val = _field_filter_del_img($val); // on supprime si il ya des images
									$term = _field_filter_taxonomy($val, $field_list[$key]['vid'], $sirtaqui_content->language);
									$content->taxonomy[$term->tid] = $term;
									$values[$values_key] = array('value' => strval($term->tid));
								}
								elseif ($field_list[$key]['type'] == 'datetime')
								{
									$values[$values_key] = _field_filter_date($val);
								}
								else // Champs texte
								{
									$val = _field_filter_img($val);
									$val = _field_filter_mail($val);
									$val = _field_filter_url($val);
									$values[$values_key] = _field_end_filter($val);
								}
							}

							// On ajoute les valeurs au contenu
							$content->$key = $values;
						}
						break;
				}
			}
		} elseif (empty($value)) {
			// On enregistre un champs vide pour effacer les anciennes valeurs
            $key = normalize_field_name('field_f_' . $key);
			$content->$key = array();
		}
	}
}

function _empty_table_value($var) {
	return (trim($var) != "");
}

/***
 * FILTRE A APPLIQUER AUX CONTENUS
 */

function _field_filter_picture($value) {

	$authorized_extensions = preg_replace('/ /', '|', variable_get('sirtaqui_authorized_extensions', "jpg jpeg png gif pdf txt").' '.strtoupper(variable_get('sirtaqui_authorized_extensions', "jpg jpeg png gif pdf txt")));

	if (preg_match('@.*(http[^"]*\.(?i:'.$authorized_extensions.')).*@', $value, $matches)) {

		// On test l'existence du fichier dans le dossier
		if (!file_exists(file_directory_path().'/sirtaqui_files/'.basename($matches[1]))) {

			// On télécharge le fichier
			$file_temp = file_get_contents($matches[1]);
			$file_temp = file_save_data($file_temp, file_directory_path().'/sirtaqui_files/'.basename($matches[1]), FILE_EXISTS_REPLACE);

		}

		// On test l'existence du fichier dans la base
		$file = _get_file_from_path(file_directory_path().'/sirtaqui_files/'.basename($matches[1]));

		if ($file) {
			return $file;
		}

  		$file = new stdClass();
		$file->filename = basename($file_temp);
		$file->filepath = $file_temp;
		$file->filemime = mime_content_type($file_temp);
		$file->filesize = filesize($file_temp);

		$file->uid = 1;
		$file->status = FILE_STATUS_PERMANENT;
		$file->timestamp = time();
		drupal_write_record('files', $file);

		return array(
			'fid' => $file->fid,
			'title' => basename($file_temp),
			'filename' => $file->filename,
			'filepath' => $file->filepath,
			'filesize' => $file->filesize,
			'filemime' => $file->filemime,
			'data' => array(
				'description' => basename($file_temp),
			),
          	'list' => 1
		); // always list
	}
	else {
		return $value;
	}
}

function _field_filter_media($value) {
	$authorized_extensions = preg_replace('/ /', '|', variable_get('sirtaqui_authorized_extensions', "jpg jpeg png gif pdf txt").' '.strtoupper(variable_get('sirtaqui_authorized_extensions', "jpg jpeg png gif pdf txt")));

	$values = array();
	if (preg_match_all('@(MEDIA_(?:[a-z0-9]|-)*\.(?:'.$authorized_extensions.'))@U', $value, $matches, PREG_SET_ORDER)) {
		// Si il y a plusieurs images extraites
		if (count($matches) > 1) {
			unset($matches[0]);
			foreach ($matches as $match)
				$values[] = variable_get("sirtaqui_media_upload_url", "http://cdt24.tourinsoft.com/upload/").$match[1];

			return $values;
		}

		return variable_get("sirtaqui_media_upload_url", "http://cdt24.tourinsoft.com/upload/").$matches[0][1];
	}

	return $value;
}

function _field_filter_del_img($val) {

	return preg_replace_callback('@<img(?i:src="[^"]+"|border="[0-9]*"|title="([^"]+)"|alt="[^"]+"|\s*)+(/>|></img>)@', create_function('$matches', 'return $matches[1];'), $val);
}



function _field_filter_taxonomy($value, $vid, $language = '') {

	if (!is_array($value)) {
		$result = db_result(db_query("SELECT tid FROM {term_data} WHERE vid = '%s' AND name = '%s' AND language = '%s'", $vid, $value, $language));
		//krumo($result);
		if (!empty($result)) {
			$term = taxonomy_get_term($result);
			return $term;
		}

		$term = array();
		$term = array(
		    'vid' => $vid, // Voacabulary ID
		    'name' => $value, // Term Name
			'language' => $language,
		);

  		taxonomy_save_term($term);

  		return (object) $term;
  		//return array( 'value' => strval($term['tid']));
	}

	return $value;
}

function _field_filter_date($value) {
	if (!is_array($value) && preg_match('@^\d{2}/\d{2}/\d{4}$@', $value))
	{
		$date = DateTime::createFromFormat('d/m/Y', $value);
		return array('value' => $date->format('Y-m-d\TH:i:s'));
	}

	return $value;
}

function _field_filter_url($value) {

	return preg_replace_callback('@Site web \(URL\)\s*:\s*(.*)@', create_function('$matches', 'return "<a href=\"http://$matches[1]\">".$matches[1]."</a>";'), $value);
}

function _field_filter_mail($value) {

	return preg_replace_callback('@Mél\s*:\s*(.*)@', create_function('$matches', 'return "<a href=\"mailto:$matches[1]\">".$matches[1]."</a>";'), $value);
}

function _field_filter_img($value) {

	return preg_replace_callback('@(?i:src="([^"]+)")@', '_import_file_and_get_url', $value);
}

function _import_file_and_get_url($matches) {

	$file_temp = file_get_contents($matches[1]);

  	$file_temp = file_save_data($file_temp, file_directory_path().'/sirtaqui_files/'.basename($matches[1]), FILE_EXISTS_REPLACE);

  	return 'src="/'.file_directory_path().'/sirtaqui_files/'.basename($matches[1]).'"';
}

function _field_end_filter($value) {
	if (!is_array($value))
	{
		return array('value' => $value);
	}

	return $value;
}

/**
 * Récupère une entrée de fichier dans la base à partir du filepath
 */
function _get_file_from_path($filepath) {

	$file = FALSE;

	$fid = db_result(db_query("SELECT fid FROM {files} WHERE filepath = '%s'", $filepath));

	if ($fid)
		$file = field_file_load($fid);

	return $file;
}