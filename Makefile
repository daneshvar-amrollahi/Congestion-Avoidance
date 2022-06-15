SENDER_DIR=sender
RECEIVER_DIR=receiver
ROUTER_DIR=router

EXECUTABLE_FILES= \
	$(SENDER_DIR)/sender.out 	\
	$(RECEIVER_DIR)/receiver.out 	\
	$(ROUTER_DIR)/router.out

.PHONY: all sender receiver router
makeall : sender receiver router

sender:
	cd sender && make

receiver:
	cd receiver && make

router:
	cd router && make

.PHONY: run_sender run_receiver run_router

run_sender: sender
	cd sender && clear && ./sender.out localhost 8001 8002

run_receiver: receiver
	cd receiver && clear && ./receiver.out localhost 8004 8003

run_router: router
	cd router && clear && ./router.out localhost 8002 8001 8003 8004