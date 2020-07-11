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
   * @brief Chain of all nodes present in the scheduler.
   */
  Chain_Control Nodes;

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
   * @brief Chain node for Scheduler_strong_APA_Context::Nodes.
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
  
  
  ---------------------------------To Change from here: -----------------------------------------------
  
/**
 * @brief Initializes the scheduler.
 *
 * @param scheduler The scheduler to initialize.
 */
void _Scheduler_strong_APA_Initialize( const Scheduler_Control *scheduler );

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
 * @brief Blocks the thread.
 *
 * @param scheduler The scheduler control instance.
 * @param[in, out] the_thread The thread to block.
 * @param[in, out] node The node of the thread to block.
 */
void _Scheduler_strong_APA_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Unblocks the thread.
 *
 * @param scheduler The scheduler control instance.
 * @param[in, out] the_thread The thread to unblock.
 * @param[in, out] node The node of the thread to unblock.
 */
void _Scheduler_strong_APA_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Updates the priority of the node.
 *
 * @param scheduler The scheduler control instance.
 * @param the_thread The thread for the operation.
 * @param[in, out] node The node to update the priority of.
 */
void _Scheduler_strong_APA_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Asks for help.
 *
 * @param  scheduler The scheduler control instance.
 * @param the_thread The thread that asks for help.
 * @param node The node of @a the_thread.
 *
 * @retval true The request for help was successful.
 * @retval false The request for help was not successful.
 */
bool _Scheduler_strong_APA_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Reconsiders help request.
 *
 * @param scheduler The scheduler control instance.
 * @param the_thread The thread to reconsider the help request of.
 * @param[in, out] node The node of @a the_thread
 */
void _Scheduler_strong_APA_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Withdraws the node.
 *
 * @param scheduler The scheduler control instance.
 * @param[in, out] the_thread The thread to change the state to @a next_state.
 * @param[in, out] node The node to withdraw.
 * @param next_state The next state for @a the_thread.
 */
void _Scheduler_strong_APA_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
);

/**
 * @brief Adds the idle thread to a processor.
 *
 * @param scheduler The scheduler control instance.
 * @param[in, out] The idle thread to add to the processor.
 */
void _Scheduler_strong_APA_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
);

/**
 * @brief Removes an idle thread from the given cpu.
 *
 * @param scheduler The scheduler instance.
 * @param cpu The cpu control to remove from @a scheduler.
 *
 * @return The idle thread of the processor.
 */
Thread_Control *_Scheduler_strong_APA_Remove_processor(
  const Scheduler_Control *scheduler,
  struct Per_CPU_Control  *cpu
);

/**
 * @brief Performs a yield operation.
 *
 * @param scheduler The scheduler control instance.
 * @param the_thread The thread to yield.
 * @param[in, out] node The node of @a the_thread.
 */
void _Scheduler_strong_APA_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSTRONGAPA_H */
