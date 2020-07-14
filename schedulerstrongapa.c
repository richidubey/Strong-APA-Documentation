/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerStrongAPA
 *
 * @brief Strong APA Scheduler Implementation
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerstrongapa.h>
#include <rtems/score/schedulersmpimpl.h>

static inline _Scheduler_strong_APA_Context *
_Scheduler_strong_APA_Get_context( const Scheduler_Control *scheduler )
{
  return (_Scheduler_strong_APA_Context *) _Scheduler_Get_context( scheduler );
}

static inline _Scheduler_strong_APA_Context *
_Scheduler_strong_APA_Get_self( Scheduler_Context *context )
{
  return (_Scheduler_strong_APA_Context *) context;
}

static inline Scheduler_strong_APA_Node *
_Scheduler_strong_APA_Node_downcast( Scheduler_Node *node )
{
  return (Scheduler_strong_APA_Node *) node;
}

void _Scheduler_strong_APA_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Chain_Initialize_empty( &self->allNodes );
}

void _Scheduler_strong_APA_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_SMP_Node *smp_node;
  
  smp_node = _Scheduler_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_initialize( scheduler, smp_node, the_thread, priority );
}

static inline void _Scheduler_strong_APA_Do_update(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   new_priority
)
{
  Scheduler_SMP_Node *smp_node;

  (void) context;

  smp_node = _Scheduler_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_update_priority( smp_node, new_priority );
}

static inline bool _Scheduler_strong_APA_Has_ready( Scheduler_Context *context )
{
  Scheduler_EDF_SMP_Context *self = _Scheduler_strong_APA_Get_self( context );
	
  bool 			ret;
  const Chain_Node      *tail;
  Chain_Node 		*next;
  
  tail = _Chain_Immutable_tail( &self->allNodes );
  next = _Chain_First( &self->allNodes );
  
  ret=false;
  
  while ( next != tail ) {
     Scheduler_strong_APA_Node *node;
       
     node = (Scheduler_strong_APA_Node *) next;
     
     if(Scheduler_SMP_Node_state( &node->Base.Base ) 
     == SCHEDULER_SMP_NODE_READY) {
     
       ret=true;
       break;
     }
  
  }
  
  return ret;
}

/**
 * @brief Checks the next highest node ready on run
 * on the CPU on which @filter node was running on 
 *
 */
static inline Scheduler_Node *_Scheduler_strong_APA_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *filter
) //TODO
{
  //Plan for this function: (Pseudo Code):
  self=_Scheduler_strong_APA_Get_self( context );
	
  Thread_Control 	thread;
  Per_CPU_Control 	thread_cpu;
  Per_CPU_Control	assigned_cpu;
  Scheduler_Node	*ret;
  Priority_Control	max_prio;
  Priority_Control	curr_prio;
	
  thread=filter->Base.Base.user;	
  thread_cpu = _Thread_Get_CPU( thread );
	
  //Implement the BFS Algorithm for task departure
  //to get the highest ready task for a particular CPU
  
  
  max_priority = _Scheduler_Node_get_priority( filter );
  max_priority = SCHEDULER_PRIORITY_PURIFY( priority );

  ret=filter;

  const Chain_Node          *tail;
  Chain_Node                *next;
  
  FiFoQueue.insert(thread_CPU);	//Help: How to implement a FifoQueue efficiently?
  visited[thread_CPU]=true;	
  
  
   while(!FiFoQueue.isEmpty) {
     tail = _Chain_Immutable_tail( &self->allNodes );
     next = _Chain_First( &self->allNodes );
  
     while ( next != tail ) {
       Scheduler_strong_APA_Node *node;
       
       node = (Scheduler_strong_APA_Node *) next;
    
       if(hasCPUinSet(	//Checks if the thread_CPU is in the affinity set of the node
         node->affinity,
         _Per_CPU_Get_index(thread_CPU)) ) {
           
         if(Scheduler_SMP_Node_state( &node->Base.Base ) 
            == SCHEDULER_SMP_NODE_SCHEDULED) {
             
            assigned_cpu = _Thread_Get_CPU( node->Base.Base.user );
            if(visited[ assigned_cpu ] == false) {
              FifoQueue.insert( assigned_cpu );
              visited[ assigned_cpu ] = true;
            } 
          }
          else if(Scheduler_SMP_Node_state( &node->Base.Base ) 
           == SCHEDULER_SMP_NODE_READY) {
            curr_priority = _Scheduler_Node_get_priority( node );
  	    curr_priority = SCHEDULER_PRIORITY_PURIFY( curr_priority );
  		
            if(curr_priority>max_priority) {
              max_priority=curr_priority;
  	      ret=node->Base.Base;
  	    }
          }
    	}
       next = _Chain_Next( next );
     }
     
     FifoQueue.pop();  
  }
  
  if( ret != filter)
  {
  	//Backtrack on the path from
  	//thread_cpu to ret, shifting along every task.
  	
  	//After this, thread_cpu receives the ret task
  	// So the ready task ret gets scheduled as well.  
  }
  
  return ret; 
}
/**
 * @brief Checks the lowest scheduled node 
 * running on a processor on which the 
 * @filter_base node could be running on
 *
 * @param context The scheduler context instance.
 * @param filter_base The node which wants to get scheduled.
 *
 * @return The lowest scheduled node on an eligible processor.
 */
static inline Scheduler_Node *_Scheduler_strong_APA_Get_lowest_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *filter_base
)
{	
  //Idea: Go through all the scheduled nodes
  //and find the lowest scheduled node
  //running on a processor which is in the
  //affinity set of filter_base

  Scheduler_Node 	*ret;
  const Chain_Node      *tail;
  Chain_Node 		*next;
  Per_CPU_Control 	curr_cpu;
  _Scheduler_strong_APA_Node *filter_node;
  Priority_Control	min_prio;
  Priority_Control	curr_prio;
  

  filter_node = _Scheduler_strong_APA_Node_downcast( filter_base );
   
  ret=NULL;
	 
  tail = _Chain_Immutable_tail( &self->allNodes );
  next = _Chain_First( &self->allNodes );
  
  while ( next != tail ) {
    Scheduler_strong_APA_Node *node;
       
    node = (Scheduler_strong_APA_Node *) next;	 
    
    if( Scheduler_SMP_Node_state( &node->Base.Base ) 
            == SCHEDULER_SMP_NODE_SCHEDULED ) {
            
      curr_cpu=_Thread_Get_CPU( node->Base.Base.user );
      
      
      if( hasCPUinSet( filter_node->affinity, //Checks if the curr_cpu is in the affinity set 
          _Per_CPU_Get_index(curr_cpu) ) ) { //of filter_node
     
        curr_priority = _Scheduler_Node_get_priority( node );
  	curr_priority = SCHEDULER_PRIORITY_PURIFY( curr_priority );
         
        if( ret == NULL ) {
          ret=node->Base.Base;		//To return atleast one ret value, this is the starting node to compare 
            			//all the nodes against.
          min_prio=curr_priority;
        }
         
        else if( curr_priority < min_prio ) {
          ret=node;
          min_prio=curr_priority;
        }
    
      }
    }
    next = _Chain_Next( next );
  }
  
  return ret;
}

static inline void _Scheduler_strong_APA_Extract_from_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  _Scheduler_strong_APA_Context     	*self;
  _Scheduler_strong_APA_Node        	*node;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_to_extract );

  _Scheduler_SMP_Extract_from_scheduled( &self->Base.Base, &node->Base.Base );
  //Not removing it from allNodes since the node could go in the ready state.
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
 
  _Assert( _Chain_Is_empty(self->allNodes) == false );
  _Assert( _Chain_Is_node_off_chain( &node->Node ) == false );
   
   _Chain_Extract_unprotected( &node->Node );	//Removed from allNodes
   _Chain_Set_off_chain( &node->Node );
}

static inline void _Scheduler_strong_APA_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Priority_Control insert_priority;

  _Scheduler_SMP_Extract_from_scheduled( context, scheduled_to_ready );
  insert_priority = _Scheduler_SMP_Node_priority( scheduled_to_ready );
  
  _Scheduler_strong_APA_Insert_ready(
    context,
    scheduled_to_ready,
    insert_priority
  );
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
  //Note: The node still stays in the allNodes chain
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
     
  _Chain_Append_unprotected( &self->allNodes, node->Node );
}

static inline void _Scheduler_strong_APA_Allocate_processor(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_base,
  Scheduler_Node    *victim_base,
  Per_CPU_Control   *victim_cpu
)
{
  _Scheduler_strong_APA_Node        *scheduled;
 
  (void) victim_base;
  scheduled = _Scheduler_strong_APA_Node_downcast( scheduled_base );

  _Scheduler_SMP_Allocate_processor_exact(
    context,
    &scheduled->Base.Base,
    NULL,
    victim_cpu
  );
}

void _Scheduler_strong_APA_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );
//The extract from ready automatically removes the node from allNodes chain.
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

static inline bool _Scheduler_strong_APA_Enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{//I'm hoping all this works on its own.
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

static inline bool _Scheduler_strong_APA_Enqueue_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{	
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

void _Scheduler_strong_APA_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Unblock(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue
  );
}

bool _Scheduler_strong_APA_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Ask_for_help(
    context,
    the_thread,
    node,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_scheduled_to_ready,
    _Scheduler_strong_APA_Get_lowest_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
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

void _Scheduler_strong_APA_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Update_priority(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Enqueue_scheduled,
    _Scheduler_strong_APA_Do_ask_for_help
  );
}

void _Scheduler_strong_APA_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Reconsider_help_request(
    context,
    the_thread,
    node,
    _Scheduler_strong_APA_Extract_from_ready
  );
}


void _Scheduler_strong_APA_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Withdraw_node(
    context,
    the_thread,
    node,
    next_state,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
}

