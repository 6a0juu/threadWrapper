#ifndef TW_SRC_CONTEXT_H
    #define TW_SRC_CONTEXT_H
    extern "C"{
        /// the context ref type
        typedef __tb_typeref__(context);

        // the context-from type
        typedef struct __tb_context_from_t
        {
            // the from-context
            tb_context_ref_t    context;

            // the passed user private data
            tb_cpointer_t       priv;

        }tb_context_from_t;

        /*! the context entry function type
        *
        * @param from          the from-context
        */
        typedef tb_void_t       (*tb_context_func_t)(tb_context_from_t from);

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
        tb_context_ref_t        tb_context_make(tb_byte_t* stackdata, tb_size_t stacksize, tb_context_func_t func);

        /*! jump to the given context 
        *
        * @param context       the to-context
        * @param priv          the passed user private data
        *
        * @return              the from-context
        */
        tb_context_from_t       tb_context_jump(tb_context_ref_t context, tb_cpointer_t priv);

    }

#endif