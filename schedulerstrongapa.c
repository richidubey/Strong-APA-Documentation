/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerStrongAPA
 *
 * @brief Strong APA Scheduler Implementation
 */

-----------------------------------------------------------------------------------
ToAdd:
    _Scheduler_strong_APA_Initialize, \  ****************************
    
    _Scheduler_strong_APA_Yield, \  ****************************
    _Scheduler_strong_APA_Block, \
    _Scheduler_strong_APA_Unblock, \
    _Scheduler_strong_APA_Update_priority, \
    _Scheduler_strong_APA_Ask_for_help, \
    _Scheduler_strong_APA_Reconsider_help_request, \
    _Scheduler_strong_APA_Withdraw_node, \
    _Scheduler_strong_APA_Add_processor, \  	****************************
    _Scheduler_strong_APA_Remove_processor, \
    _Scheduler_strong_APA_Node_initialize, \      ****************************
    _Scheduler_strong_APA_Set_affinity \	********************************
  
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

static inline _Scheduler_strong_APA_Context *
_Scheduler_strong_APA_Get_context( const Scheduler_Control *scheduler )
{
  return (_Scheduler_strong_APA_Context *) _Scheduler_Get_context( scheduler );
}


void _Scheduler_strong_APA_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Chain_Initialize_empty( &self->readyNodes );
  _Chain_Initialize_empty( &self->scheduledNodes );
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

   _Chain_Append_unprotected( &self->readyNodes, &self_node->Node );	//Is it right to put it in the ready chain right away?
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


static inline void _Scheduler_strong_APA_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  _Scheduler_strong_APA_Context     *self;
  _Scheduler_strong_APA_Node        *node;


  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_to_extract );
 
  _Assert( _Chain_Is_empty(self->readyNodes) == false );
  _Assert( _Chain_Is_node_off_chain( &node->Node ) == false );
   
   _Chain_Extract_unprotected( &node->Node );
   _Chain_Set_off_chain( &node->Node );
 }



static inline Scheduler_Node *_Scheduler_strong_APA_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *filter
)	//TODO
{
/*  _Scheduler_strong_APA_Context *self;*/
/*  _Scheduler_strong_APA_Node    *highest_ready;*/
/*  _Scheduler_strong_APA_Node    *node;*/
/*  const Chain_Node          *tail;*/
/*  Chain_Node                *next;*/

/*  self = _Scheduler_EDF_SMP_Get_self( context );*/
/*  highest_ready = (Scheduler_EDF_SMP_Node *)*/
/*    _RBTree_Minimum( &self->Ready[ 0 ].Queue );*/
/*  _Assert( highest_ready != NULL );*/

/*  tail = _Chain_Immutable_tail( &self->Affine_queues );*/
/*  next = _Chain_First( &self->Affine_queues );*/

/*  while ( next != tail ) {*/
/*    Scheduler_EDF_SMP_Ready_queue *ready_queue;*/

/*    ready_queue = (Scheduler_EDF_SMP_Ready_queue *) next;*/
/*    highest_ready = _Scheduler_EDF_SMP_Challenge_highest_ready(*/
/*      self,*/
/*      highest_ready,*/
/*      &ready_queue->Queue*/
/*    );*/

/*    next = _Chain_Next( next );*/
/*  }*/

/*  return &highest_ready->Base.Base;*/
}


 
static inline void  _Scheduler_strong_APA_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Priority_Control insert_priority;

  _Scheduler_strong_APA_Extract_from_ready( context, ready_to_scheduled );
  insert_priority = _Scheduler_SMP_Node_priority( ready_to_scheduled );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
  _Scheduler_SMP_Insert_scheduled(
    context,
    ready_to_scheduled,
    insert_priority
  );
  
  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_base );
  _Chain_Append_unprotected( &self->scheduledNodes, node->Node );  
}



static inline void _Scheduler_strong_APA_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Priority_Control insert_priority;

  _Scheduler_SMP_Extract_from_scheduled( context, scheduled_to_ready );
  insert_priority = _Scheduler_SMP_Node_priority( scheduled_to_ready );
 
  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( scheduled_to_ready );
 
  _Assert( _Chain_Is_empty(self->scheduledNodes) == false );
  _Assert( _Chain_Is_node_off_chain( &node->Node ) == false );
   
  _Chain_Extract_unprotected( &node->Node );
  _Chain_Set_off_chain( &node->Node ); 
  
  _Scheduler_strong_APA_Insert_ready(
    context,
    scheduled_to_ready,
    insert_priority
  );
 
}

static inline Scheduler_Node *_Scheduler_strong_APA_Get_lowest_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *filter_base
)
{	//Checkout about this function.
/*  Scheduler_EDF_SMP_Node *filter;*/
/*  uint8_t                 rqi;*/

/*  filter = _Scheduler_EDF_SMP_Node_downcast( filter_base );*/
/*  rqi = filter->ready_queue_index;*/

/*  if ( rqi != 0 ) {*/
/*    Scheduler_EDF_SMP_Context *self;*/
/*    Scheduler_EDF_SMP_Node    *node;*/

/*    self = _Scheduler_EDF_SMP_Get_self( context );*/
/*    node = _Scheduler_EDF_SMP_Get_scheduled( self, rqi );*/

/*    if ( node->ready_queue_index > 0 ) {*/
/*      _Assert( node->ready_queue_index == rqi );*/
/*      return &node->Base.Base;*/
/*    }*/
/*  }*/

/*  return _Scheduler_SMP_Get_lowest_scheduled( context, filter_base );*/
}

static inline void _Scheduler_strong_APA_Insert_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  Priority_Control   insert_priority
)
{
  _Scheduler_strong_APA_Context     *self;
  _Scheduler_strong_APA_Node        *node;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_base );
  
  _Assert( _Chain_Is_node_off_chain( &node->Node ) == true );
     
  _Chain_Append_unprotected( &self->readyNodes, node->Node );
 
}

static inline void _Scheduler_strong_APA_Allocate_processor(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_base,
  Scheduler_Node    *victim_base,
  Per_CPU_Control   *victim_cpu
)
{
  _Scheduler_strong_APA_Context     *self;
  _Scheduler_strong_APA_Node        *scheduled;
  

  (void) victim_base;
  self = _Scheduler_strong_APA_Get_self( context );
  scheduled = _Scheduler_strong_APA_Node_downcast( scheduled_base );
  rqi = scheduled->ready_queue_index;

  if ( rqi != 0 ) {
    Scheduler_EDF_SMP_Ready_queue *ready_queue;
    Per_CPU_Control               *desired_cpu;

    ready_queue = &self->Ready[ rqi ];

    if ( !_Chain_Is_node_off_chain( &ready_queue->Node ) ) {
      _Chain_Extract_unprotected( &ready_queue->Node );
      _Chain_Set_off_chain( &ready_queue->Node );
    }

    desired_cpu = _Per_CPU_Get_by_index( rqi - 1 );

    if ( victim_cpu != desired_cpu ) {
      Scheduler_EDF_SMP_Node *node;

      node = _Scheduler_EDF_SMP_Get_scheduled( self, rqi );
      _Assert( node->ready_queue_index == 0 );
      _Scheduler_strong_APA_Set_scheduled( self, node, victim_cpu );
      _Scheduler_SMP_Allocate_processor_exact(
        context,
        &node->Base.Base,
        NULL,
        victim_cpu
      );
      victim_cpu = desired_cpu;
    }
  }

  _Scheduler_EDF_SMP_Set_scheduled( self, scheduled, victim_cpu );
  _Scheduler_SMP_Allocate_processor_exact(
    context,
    &scheduled->Base.Base,
    NULL,
    victim_cpu
  );
}

static inline bool _Scheduler_strong_APA_Enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{//TODO: Checkout what this is supposed to do
  return _Scheduler_SMP_Enqueue(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_scheduled_to_ready,
    _Scheduler_strong_APA_Get_lowest_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
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
      _Scheduler_strong_APA_Extract_from_ready,		
      _Scheduler_strong_APA_Get_highest_ready,	
      _Scheduler_strong_APA_Move_from_ready_to_scheduled,
      _Scheduler_strong_APA_Enqueue,
      _Scheduler_strong_APA_Allocate_processor
    );

  return true;
}

void _Scheduler_strong_APA_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Add_processor(
    context,
    idle,
    _Scheduler_strong_APA_Has_ready,
    _Scheduler_strong_APA_Enqueue_scheduled,
    _Scheduler_strong_APA_Register_idle
  );
}

static inline bool _Scheduler_strong_APA_Has_ready( Scheduler_Context *context )
{
  Scheduler_EDF_SMP_Context *self = _Scheduler_strong_APA_Get_self( context );

  return !_Chain_Is_empty(self->readyNodes);
}

static inline bool _Scheduler_strong_APA_Enqueue_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{	//TODO: Check if you need to add the node to the scheduled node's chain list
  return _Scheduler_SMP_Enqueue_scheduled(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
}

static inline void _Scheduler_strong_APA_Register_idle(
  Scheduler_Context *context,
  Scheduler_Node    *idle_base,
  Per_CPU_Control   *cpu
)
{
  _Scheduler_strong_APA_Context *self;
  _Scheduler_strong_APA_Node    *idle;

  self = _Scheduler_strong_APA_Get_self( context );
  idle = _Scheduler_strong_APA_Node_downcast( idle_base );
  _Scheduler_strong_APA_Set_scheduled( self, idle, cpu );
}

static inline void _Scheduler_strong_APA_Set_scheduled(
  Scheduler_strong_APA_Context 	*self,
  Scheduler_strong_APA_Node   	*scheduled,
  const Per_CPU_Control     	*cpu
)
{
  self->CPU[ _Per_CPU_Get_index( cpu ) ].scheduled = scheduled;
}

void _Scheduler_strong_APA_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Yield(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Enqueue_scheduled
  );
}

void _Scheduler_strong_APA_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Block(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
}

static inline void _Scheduler_strong_APA_Extract_from_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  _Scheduler_strong_APA_Context     	*self;
  _Scheduler_strong_APA_Node        	*node;
  uint8_t                        	 rqi;
  _Scheduler_strong_APA_Ready_queue 	*ready_queue;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_to_extract );

  _Scheduler_SMP_Extract_from_scheduled( &self->Base.Base, &node->Base.Base ); 
}

Thread_Control *_Scheduler_strong_APA_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Enqueue
  );
}

