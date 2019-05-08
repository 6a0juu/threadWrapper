#ifndef TW_SRC_CONTEXT_H
    #define TW_SRC_CONTEXT_H
    #ifdef __cplusplus
        extern "C"{
    #endif
            /// the context ref type
            //typedef __tb_typeref__(context);
            typedef struct __tb_context_dummy_t{
                signed int dummy;
            } const* context_ref_t;
            // the context-from type
            typedef struct __tb_context_from_t
            {
                // the from-context
                context_ref_t    context;

                // the passed user private data
                const void*       priv;

            }context_from_t;

            /*! the context entry function type
            *
            * @param from          the from-context
            */
            typedef void (*context_func_t) (context_from_t from);

            /* //////////////////////////////////////////////////////////////////////////////////////
            * interfaces
            */

            /*! make context from the given the stack space and the callback function
            *
            * @param stackdata     the stack data
            * @param stacksize     the stack size
            * @param func          the entry function
            *
            * @return              the context pointer
            */
            context_ref_t        context_make(unsigned char* stackdata, unsigned long stacksize, context_func_t func);

            /*! jump to the given context 
            *
            * @param context       the to-context
            * @param priv          the passed user private data
            *
            * @return              the from-context
            */
            context_from_t       context_jump(context_ref_t context, const void* priv);
    #ifdef __cplusplus
        }
    #endif
#endif