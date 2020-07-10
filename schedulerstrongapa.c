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
 * @param scheduler Scheduler_Control used to get 
 * reference to Strong APA scheduler context 
 *
 * @see _Scheduler_strong_APA_Node_initialize()
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
