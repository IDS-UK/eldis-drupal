<?php

/**
 * @file
 * Default rule configurations for License.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function eldis_job_default_rules_configuration() {
  $rules = array();
/*
  // Add a reaction rule to suspend a newly created commerce license
  $rule = rules_reaction_rule();
  $rule->label = t('Before adding a product to the cart');
  $rule->tags = array('Eldis job');
  $rule->active = TRUE;
  $rule->weight = -10;
  $rule
    ->event('commerce_line_item_presave')
    ->action('eldis_job_license_prevent_duplicates', array(
      'commerce_line_item:select' => 'commerce-line-item',
    ));

  $rules['eldis_job_license_prevent_duplicates'] = $rule;
*/

  // Add a reaction rule to revoke the license of a pay per job user, after initial job creation
  $rule = rules_reaction_rule();
  $rule->label = t('After adding a job, if the user has a pay per job license, revoke it');
  $rule->tags = array('Eldis job');
  $rule->active = TRUE;
  $rule->weight = 9;
  $rule
    ->event('node_insert')
    ->condition('data_is', array('data:select' => 'node:type', 'value' => 'job'))
    ->action('eldis_job_license_revoke_pay_per_job', array(
      'data:select' => 'node',
    ));

  $rules['eldis_job_license_revoke_after_add_single_job'] = $rule;

  return $rules;
}


/**
 * Rules action callback: eldis_license_sponsored_new_suspended_callback
 * @param $commerce_license
 */
function eldis_license_sponsored_new_suspended_callback($commerce_license) {

  // @todo send an email... or do this in the UI....

}