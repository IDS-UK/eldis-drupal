<?php

/**
 * @file
 * Default rule configurations for License.
 */

/**
 * Implements hook_default_rules_configuration().
 *
 * Add
 */
function eldis_job_default_rules_configuration() {
  $rules = array();

  // Add a reaction rule to suspend a newly created commerce license
  $rule = rules_reaction_rule();

  $rule->label = t('After creating a license that references the "Sponsored" prooduct, suspend the license');
  $rule->tags = array('Eldis job');
  $rule->active = TRUE;
  // @todo check what the weight should be
  $rule->weight = 9;

  $rule
    ->event('commerce_license_insert') // it was easier to dig to get the referenced product in the action function
    ->action('eldis_job_license_suspend_sponsored', array(
      'commerce_license:select' => 'commerce-license',
    ));

  $rules['eldis_job_license_suspend_after_save_new_sponsored'] = $rule;


  // Add a reaction rule to revoke the license of a pay per job user, after initial job creation
  $rule = rules_reaction_rule();

  $rule->label = t('After adding a job, if the user has a pay per job license, revoke it');
  $rule->tags = array('Eldis job');
  $rule->active = TRUE;
  // Run after the rules that modify the order owner.
  $rule->weight = 9;

  $rule
    ->event('node_insert')
    ->condition('data_is', array('data:select' => 'node:type', 'value' => 'job')) // correct
    ->action('eldis_job_license_revoke_pay_per_job', array(
      'commerce_license:select' => 'commerce-license',
    ));

  $rules['eldis_job_license_revoke_after_add_single_job'] = $rule;

  return $rules;
}
