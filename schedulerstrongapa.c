-----------------------------------------------------------------------------------
ToAdd:
    _Scheduler_strong_APA_Initialize, \  --------------
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
  
    SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
    
    
    Refer from
    _Scheduler_EDF_SMP_Initialize, \
    _Scheduler_default_Schedule, \
    _Scheduler_EDF_SMP_Yield, \
    _Scheduler_EDF_SMP_Block, \
    _Scheduler_EDF_SMP_Unblock, \
    _Scheduler_EDF_SMP_Update_priority, \
    _Scheduler_EDF_Map_priority, \
    _Scheduler_EDF_Unmap_priority, \
    _Scheduler_EDF_SMP_Ask_for_help, \
    _Scheduler_EDF_SMP_Reconsider_help_request, \
    _Scheduler_EDF_SMP_Withdraw_node, \
    _Scheduler_EDF_SMP_Pin, \
    _Scheduler_EDF_SMP_Unpin, \
    _Scheduler_EDF_SMP_Add_processor, \
    _Scheduler_EDF_SMP_Remove_processor, \
    _Scheduler_EDF_SMP_Node_initialize, \
    _Scheduler_default_Node_destroy, \
    _Scheduler_EDF_Release_job, \
    _Scheduler_EDF_Cancel_job, \
    _Scheduler_default_Tick, \
    _Scheduler_EDF_SMP_Start_idle, \
    _Scheduler_EDF_SMP_Set_affinity \
  -------------------------------------------------------------------------------------------
  
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <rtems/score/schedulerstrongapa.h>
#include <rtems/score/schedulersmpimpl.h>

static inline Scheduler_strong_APA_Node *
_Scheduler_strong_APA_Node_downcast( Scheduler_Node *node )
{
  return (Scheduler_strong_APA_Node *) node;
}

static inline Scheduler_EDF_SMP_Context *
_Scheduler_strong_APA_Get_context( const Scheduler_Control *scheduler )
{
  return (_Scheduler_strong_APA_Context *) _Scheduler_Get_context( scheduler );
}

/**
 * Initializes the Strong_APA scheduler.
 *
 * Sets the chain containing all the nodes to empty 
 * and initializes the SMP scheduler.
 *
 * @param scheduler used to get 
 * reference to Strong APA scheduler context 
 *
 * @return void
 *
 * @see _Scheduler_strong_APA_Node_initialize()
 *
 *
 */
void _Scheduler_strong_APA_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Chain_Initialize_empty( &self->Nodes );
}

void _Scheduler_strong_APA_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
 Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_context( scheduler );

  Scheduler_SMP_Node *smp_node;
  Scheduler_strong_APA_Node *self_node;

  self_node=_Scheduler_strong_APA_Node_downcast(node);

  smp_node = _Scheduler_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_initialize( scheduler, smp_node, the_thread, priority );

   _Chain_Append_unprotected( &self->Nodes, &self_node->Node );
}

static inline void  _Scheduler_strong_APA_Do_set_affinity(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  void              *arg
)
{
  _Scheduler_strong_APA_Node *node;
  const Processor_mask       *affinity;

  node = _Scheduler_strong_APA_Node_downcast( node_base );
  affinity = arg;
  node->affinity = *affinity;
}

bool _Scheduler_strong_APA_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
)
{
  Scheduler_Context     	*context;
  Scheduler_strong_APA_Node 	*node;
  Processor_mask          	local_affinity;
  States_Control          	current_state;

  context = _Scheduler_Get_context( scheduler );
  _Processor_mask_And( &local_affinity, &context->Processors, affinity );

  if ( _Processor_mask_Is_zero( &local_affinity ) ) {
    return false;
  }

  node = Scheduler_strong_APA_Node_downcast( node_base );

  if ( _Processor_mask_Is_equal( &node->affinity, affinity ) )
    return true;	//Nothing to do. Return true.

    _Scheduler_SMP_Set_affinity(
      context,
      thread,
      node_base,
      &local_affinity,
      _Scheduler_strong_APA_Do_set_affinity,
      _Scheduler_strong_APA_Extract_from_ready,		//--------------------------INCOMPLETE FROM HERE DOWNWARDS TILL Line no 96.
      _Scheduler_strong_APA_Get_highest_ready,	//TODO Next! We would also need the info of the cpu the node was executing on.
      _Scheduler_strong_APA_Move_from_ready_to_scheduled,
      _Scheduler_strong_APA_Enqueue,
      _Scheduler_strong_APA_Allocate_processor
    );


  return true;
}
