<?php

/**
 * @file
 * Eldis jobs rules definitions.
 */

/**
 * Implements hook_rules_event_info().
 *
 * Define some events that can be used to trigger actions
 * in the Rules UI.
 */
function eldis_job_rules_event_info() {
  $events = array();

  /*
  $events['eldis_license_sponsored_new_suspended'] = array(
    'label' => t('After suspending a newly created sponsored license'),
    'group' => t('Eldis job'),
    'variables' => array(
      'commerce_license' => array(
        'type' => 'commerce_license',
        'label' => t('The license'),
      ),
    ),
  );
*/
  return $events;
}

/**
 * Implements hook_rules_action_info().
 */
function eldis_job_rules_action_info() {
  $actions = array();

  $actions['eldis_job_license_revoke_pay_per_job'] = array(
    'label' => t('Revoke a "Pay per job" license'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('The node that we have just added.'),
      ),
    ),
    'group' => t('Eldis job'),
    'callbacks' => array(
      'execute' => 'eldis_job_license_revoke_pay_per_job',
    ),
  );

  return $actions;
}

/**
 * Implements hook_rules_action_info_alter()
 *
 * We need to alter some default rules set by commerce_license
 * In order to deal with the
 */
function eldis_job_rules_action_info_alter(&$actions) {
  $actions['commerce_license_activate_order_licenses']['callbacks']['execute'] = 'eldis_job_activate_order_licenses';
}
