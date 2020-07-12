/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerStrongAPA
 *
 * @brief Strong APA Scheduler API
 */


#ifndef _RTEMS_SCORE_SCHEDULERSTRONGAPA_H
#define _RTEMS_SCORE_SCHEDULERSTRONGAPA_H

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulerpriority.h>
#include <rtems/score/schedulersmp.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreSchedulerStrongAPA Strong APA Scheduler
 *
 * @ingroup RTEMSScoreSchedulerSMP
 *
 * @brief Strong APA Scheduler
 *
 * This is an implementation of the Whahahahahaha
 *
 * WhahahahahHHHAAAAHHHAA
 *
 * @{
 */
 
  /**
 * @brief CPU Structure for Strong APA
 * scheduler.
 *
 * This structure stores nodes associated
 * with a CPU and the currently scheduled node.
 */
 typedef struct {
  /**
   * @brief Chain of nodes that have affinity to this CPU.
   */
  Chain_Control affineNode;

  /**
   * @brief The scheduled thread of the corresponding processor.
   */
  Scheduler_strong_APA_Node *scheduled;
} Scheduler_strong_APA_CPU;

 /**
 * @brief Scheduler context for Strong APA
 * scheduler.
 *
 * Has the structure for scheduler context
 * and Node defintion for Strong APA scheduler
 */
 typedef struct {
 /**
   * @brief SMP Context to refer to SMP implementation
   * code.  
   */
  Scheduler_SMP_Context Base;
  
  /**
   * @brief A table with structure for each CPU.
   *
   * The index correspond to the CPU index, 
   * so index 0 points to CPU 0 and so on. 
   */
  Scheduler_strong_APA_CPU CPU[ RTEMS_ZERO_LENGTH_ARRAY ];

} Scheduler_strong_APA_Context;

/**
 * @brief Scheduler node specialization for Strong APA
 * schedulers.
 */
typedef struct {
  /**
   * @brief SMP scheduler node.
   */
  Scheduler_SMP_Node Base;
  
 /**
   * @brief Chain node for 
   * Scheduler_strong_APA_Context::
   * Scheduler_strong_APA_CPU::
   * affineNode.
   */
  Chain_Node Node;

  /**
   * @brief The associated affinity set of this node.
   */
  Processor_mask affinity;
} Scheduler_strong_APA_Node;

/**
 * @brief Entry points for the Strong APA Scheduler.
 */
#define SCHEDULER_STRONG_APA_ENTRY_POINTS \
  { \
    _Scheduler_strong_APA_Initialize, \
    _Scheduler_default_Schedule, \
    _Scheduler_strong_APA_Yield, \
    _Scheduler_strong_APA_Block, \
    _Scheduler_strong_APA_Unblock, \
    _Scheduler_strong_APA_Update_priority, \
    _Scheduler_default_Map_priority, \
    _Scheduler_default_Unmap_priority, \
    _Scheduler_strong_APA_Ask_for_help, \
    _Scheduler_strong_APA_Reconsider_help_request, \
    _Scheduler_strong_APA_Withdraw_node, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_strong_APA_Add_processor, \
    _Scheduler_strong_APA_Remove_processor, \
    _Scheduler_strong_APA_Node_initialize, \
    _Scheduler_default_Node_destroy, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Cancel_job, \
    _Scheduler_default_Tick, \
    _Scheduler_SMP_Start_idle \
    _Scheduler_strong_APA_Set_affinity \
  }
  
/**
 * @brief Initializes the Strong_APA scheduler.
 *
 * Sets the chain containing all the nodes to empty 
 * and initializes the SMP scheduler.
 *
 * @param scheduler used to get 
 * reference to Strong APA scheduler context 
 * @return void
 * @see _Scheduler_strong_APA_Node_initialize()
 *
 */
void _Scheduler_strong_APA_Initialize( 
   const Scheduler_Control *scheduler 
   );

/**
 * @brief Initializes the node with the given priority.
 *
 * @param scheduler The scheduler control instance.
 * @param[out] node The node to initialize.
 * @param the_thread The thread of the node to initialize.
 * @param priority The priority for @a node.
 */
void _Scheduler_strong_APA_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 * @brief Removes the ready node from the Chain of nodes
 *
 * @param context The scheduler context instance.
 * @param node_to_extract The node to remove from the chain.
 */
static inline void _Scheduler_strong_APA_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
);

/**
 * @brief Checks if the processor set of the scheduler is the subset of the affinity set.
 *
 * Default implementation of the set affinity scheduler operation.
 *
 * @param scheduler This parameter is unused.
 * @param thread This parameter is unused.
 * @param node This parameter is unused.
 * @param affinity The new processor affinity set for the thread.
 *
 * @see _Scheduler_strong_APA_Do_set_affinity()
 *
 * @retval true The processor set of the scheduler is a subset of the affinity set.
 * @retval false The processor set of the scheduler is not a subset of the affinity set.
 */
bool _Scheduler_strong_APA_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
);

static inline Scheduler_Node *_Scheduler_strong_APA_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *filter
);

static inline void  _Scheduler_strong_APA_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSTRONGAPA_H */
