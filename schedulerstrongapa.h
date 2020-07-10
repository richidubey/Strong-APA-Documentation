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
