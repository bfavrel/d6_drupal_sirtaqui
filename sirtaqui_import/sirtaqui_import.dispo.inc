<?php
// $Id$

/**
 * @file
 * Creation sirtaqui import dispos
 */

function sirtaqui_import_update_dispos($node, $module_id, $infos) {
	
	$dispos_results = _sirtaqui_get_content_dispo($module_id, $infos);
	
	$date_field = 'DISPO_PLANNING_DATE';
	$state_field = 'DISPO_PLANNING_ETAT';
	
	$dispos = array();
	if ($dispos_results['NewDataSet']['DISPO']) {
		foreach ($dispos_results['NewDataSet']['DISPO'] as $dispo_occ) {
			$date = DateTime::createFromFormat('d/m/Y', $dispo_occ[$date_field]);
			$dispos[$dispo_occ[$state_field]][] = $date->getTimestamp();
		}
	}
	
	if ($dispos) {
		module_load_include('inc', 'prestataires_dispos', 'prestataires_dispos.edit');
		
		$sirtaqui_dispo_types = prestataires_dispos_get_sirtaqui_dispos_types();
		$dispos_types_en = prestataires_dispos_get_dispos_types();
		
		foreach ($dispos as $state => $dispo) {
			$type = array_search($state, $sirtaqui_dispo_types);
			if (!$type) {continue;}
			$periodes = _sirtaqui_dispos_get_periodes($dispo);
			foreach ($periodes as $periode) {
				prestataires_dispos_save($node->nid, t($dispos_types_en[$type - 1]), $periode['start'], $periode['end']);
			}
		}
	}
	
}

/**
 *
 * Récupère les dispos d'une fiche
 * @param string $module_id : l'identifiant du flux
 * @param array $infos : tableau contenant les champs du listing pour un contenu
 */
function _sirtaqui_get_content_dispo($module_id, $infos) {

	// On récupère le dispos de la fiche
	$attrs['idModule'] = $module_id;
	$attrs['OBJETTOUR_CODE'] = $infos['OBJETTOUR_CODE'];
	$attrs['idOffre'] = $infos['ID'];
	$dispos_results = sirtaqui_make_soap_request(variable_get('sirtaqui_feed_url', ''), variable_get('sirtaqui_dispo_request', ''), $attrs);
	
	$method = variable_get('sirtaqui_dispo_request', '').'Result';
	
	//translate response to array
	$xml = simplexml_load_string($dispos_results->$method->any);
	//krumo($xml);
	$json = json_encode($xml);
	$dispos = json_decode($json,TRUE);

	if (!$dispos) {
		return array();
	}

	return $dispos;
}

/**
 * On récupère un tableau de date et on renvoie un tableau de période
 * @param array $dates
 */
function _sirtaqui_dispos_get_periodes($dates) {
	
	if (!$dates) {
		return array();
	}
	sort($dates, SORT_NUMERIC);
	
	$periodes = array();
	$periodes[] = array(
		'start' => $dates[0],
		'end' => $dates[0]
	);
	
	$periode_occ = 0;
	$size = count($dates);
	for ($i = 1; $i < $size; $i++) {
		if ($periodes[$periode_occ]['end'] + 86400 == $dates[$i]) {
			$periodes[$periode_occ]['end'] = $dates[$i];
		}
		else {
			$periodes[] = array(
				'start' => $dates[$i],
				'end' => $dates[$i]
			);
			$periodes[$periode_occ]['start'] = date('Y-m-d', $periodes[$periode_occ]['start']);
			$periodes[$periode_occ]['end'] = date('Y-m-d', $periodes[$periode_occ]['end']);
			$periode_occ++;
		}
	}
	
	$periodes[$periode_occ]['start'] = date('Y-m-d', $periodes[$periode_occ]['start']);
	$periodes[$periode_occ]['end'] = date('Y-m-d', $periodes[$periode_occ]['end']);
	
	return $periodes;
}