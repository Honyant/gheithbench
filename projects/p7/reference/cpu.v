`timescale 1ps/1ps
module main();
    initial begin
        $dumpfile("cpu.vcd");
        $dumpvars(0,main);
    end

    reg [31:0] count = 0;
    wire clk;
    clock c0(clk);
    wire halt = wb_halt && val_wb;
    counter counter(halt, clk);
    reg [15:0] pc = 16'h0000;
    reg [15:0] ins_lower_word = 0;
    reg finish_stall = 0;
    wire [15:0] ins = last_stall ? ins :
                        finish_stall ? (pc[0] == 1 ? {prev_fetched_ins[7:0], ins_lower_word[15:8]} : prev_fetched_ins) : 
                        (pc[0] == 1 ? {ins_raw[7:0], ins_lower_word[15:8]} : ins_raw);
    wire [15:0] ins_raw;
    reg [15:0] prev_fetched_ins = 0;
    wire branch;
    wire flush;
    wire stall;

    reg val_f1 = 1;
    reg val_f2 = 0;
    reg val_d = 0;
    reg val_e1 = 0;
    reg val_e2 = 0;
    reg val_wb = 0;
    reg[15:0] prev_pc = 0;
    reg[15:0] prev_lower_word = 0;
    reg[15:0] f2_pc = 0;
    reg[15:0] d_pc = 0;
    reg[15:0] e1_pc = 0;
    reg[15:0] e2_pc = 0;
    reg[15:0] wb_pc = 0;

    reg f1_hijack = 0;
    reg f2_hijack = 0;
    reg last_stall = 0;
    reg last_last_stall = 0;

    always @(posedge clk) begin
        prev_pc <= pc;
        val_f2 <= stall ? val_f2 : val_f1 && !(branch || flush);
        val_d <= stall ? val_d : val_f2 && !(branch || flush);
        val_e1 <= (val_e1 && misaligned_st ? 1 : val_e1 && misaligned_ld ? 1 : stall ? 0 : val_d ) && !(branch || flush);
        val_e2 <= (val_e1 && misaligned_ld ? 0 : val_e1) && !(branch || flush);
        val_wb <= val_e2 && !(branch || flush);

        f2_pc <= stall ? f2_pc : pc;
        d_pc <= stall ? d_pc : f2_pc;
        e1_pc <= d_pc;
        e2_pc <= e1_pc;
        wb_pc <= e2_pc;

        f1_hijack <= jump_misaligned;
        f2_hijack <= f1_hijack;
        ins_lower_word <= f2_hijack ? mem_read_raw : ins_raw;
        prev_fetched_ins <= ins_raw;
        last_stall <= stall;
        last_last_stall <= last_stall;
        finish_stall <= (prev_pc == pc);
    end

// --------------- decode
    wire[3:0] opcode1 = ins[15:12];
    wire[3:0] opcode2 = ins[7:4];
    wire[3:0] ra = ins[11:8];
    wire[3:0] rb = ins[7:4];
    wire[3:0] rt = ins[3:0];
    wire[3:0] reg1 = ra;
    wire[3:0] reg2 = (opcode1 == 4'b0000) ? rb : rt;
    //encode which instruction using numbers
    wire[15:0] op = (opcode1 == 4'b0000) ? 0: // sub
            (opcode1 == 4'b1000) ? 1: // movl
            (opcode1 == 4'b1001) ? 2: // movh
            (opcode1 == 4'b1110 && opcode2 == 4'b0000) ? 3: // jz
            (opcode1 == 4'b1110 && opcode2 == 4'b0001) ? 4: // jnz
            (opcode1 == 4'b1110 && opcode2 == 4'b0010) ? 5: // js
            (opcode1 == 4'b1110 && opcode2 == 4'b0011) ? 6: // jns
            (opcode1 == 4'b1111 && opcode2 == 4'b0000) ? 7: // ld
            (opcode1 == 4'b1111 && opcode2 == 4'b0001) ? 8: // st
            9;

    wire load_hazard = val_d && val_e1 && (e1_op == 7 ? (op == 7 && ra == e1_rt_adr) : 0);
    assign stall = !(branch || flush) && (!(last_stall) && load_hazard || misaligned_ld || misaligned_st); //get rid of last stall
    // decode->e1 registers
    reg[15:0] e1_op = 0;
    reg[3:0] e1_rt_adr = 0;
    reg[7:0] e1_imm = 0;
    reg[3:0] e1_reg1_adr = 0;
    reg[3:0] e1_reg2_adr = 0;

    always @(posedge clk) begin
        e1_op <=  misaligned_ld || misaligned_st ? e1_op : op; // Default case
        e1_rt_adr <= misaligned_ld || misaligned_st ? e1_rt_adr : rt;
        e1_reg1_adr <= misaligned_ld || misaligned_st ? e1_reg1_adr : reg1;
        e1_reg2_adr <= misaligned_ld || misaligned_st ? e1_reg2_adr : reg2;
        e1_imm <= misaligned_ld || misaligned_st ? e1_imm : ins[11:4];
    end

// --------------- e1
    wire[15:0] reg1_raw_out;
    wire[15:0] reg2_raw_out;
    // e1->e2 registers
    reg[15:0] e2_op = 0;
    reg[7:0] e2_imm = 0;
    reg[3:0] e2_rt_adr = 0;
    reg[3:0] e2_reg1_adr = 0;
    reg[3:0] e2_reg2_adr = 0;
    reg[15:0] e2_reg1_raw_val = 0;
    reg[15:0] e2_reg2_raw_val = 0;
    wire[15:0] forwarded_e1_val = e1_misaligned_ld || e1_misaligned_st ? misaligned_adr + 2 : (e1_reg1_adr == 0) ? 0 : val_e1 && val_e2 && e1_reg1_adr == e2_rt_adr ? e2_result : write_reg_en && (e1_reg1_adr == wb_rt_adr) ? write_val : reg1_raw_out;
    wire[15:0] mem_read_adr = forwarded_e1_val;
    wire misaligned_ld = val_e1 && e1_op == 7 && (forwarded_e1_val[0] == 1) && !e1_misaligned_ld; // misaligned load
    wire misaligned_st = val_e1 && e1_op == 8 && (forwarded_e1_val[0] == 1) && !e1_misaligned_st; // misaligned store

    reg e1_misaligned_ld = 0;
    reg e2_misaligned_ld = 0;
    reg wb_misaligned_ld = 0;

    reg e1_misaligned_st = 0;
    reg e2_misaligned_st = 0;
    reg wb_misaligned_st = 0;
    reg wb2_misaligned_st = 0;

    reg[15:0] misaligned_adr = 0;

    always @(posedge clk) begin
        misaligned_adr <= misaligned_ld || misaligned_st? forwarded_e1_val : 0;
        e1_misaligned_ld <= misaligned_ld;
        e2_misaligned_ld <= e1_misaligned_ld;
        wb_misaligned_ld <= e2_misaligned_ld;

        e1_misaligned_st <= misaligned_st;
        e2_misaligned_st <= misaligned_st;
        wb_misaligned_st <= e2_misaligned_st;
        wb2_misaligned_st <= wb_misaligned_st;

        e2_op <= e1_op;
        e2_imm <= e1_imm;
        e2_rt_adr <= e1_rt_adr;
        e2_reg1_adr <= e1_reg1_adr;
        e2_reg2_adr <= e1_reg2_adr;
        e2_reg1_raw_val <= forwarded_e1_val;
        e2_reg2_raw_val <= (e1_reg2_adr == 0) ? 0 :val_e1 && val_e2 && e1_reg2_adr == e2_rt_adr ? e2_result : write_reg_en && (e1_reg2_adr == wb_rt_adr) ? write_val : reg2_raw_out;
    end
// --------------- e2
    
    reg[15:0] wb_op = 0;
    reg[3:0] wb_rt_adr = 0;
    reg[15:0] wb_reg1_adr = 0;
    reg[15:0] wb_reg2_adr = 0;
    reg[15:0] wb_reg1_val = 0;
    reg[15:0] wb_reg2_val = 0;
    reg[15:0] wb_result = 0;
    reg wb_branch = 0;
    reg wb_halt = 0;

    wire mem_forwarded = (val_e2 && e2_op == 7) && write_mem_en && (e2_reg1_raw_val[15:1] == wb_reg1_val[15:1]);
    wire[15:0] mem_forward_val = mem_forwarded ? write_val : 0;
    reg wb_mem_forwarded = 0;
    reg[15:0] wb_mem_forward_val = 0;

    wire[15:0] reg1_out = (e2_reg1_adr == 0) ? 0 : write_reg_en && (e2_reg1_adr == wb_rt_adr) ? write_val : e2_reg1_raw_val;
    wire[15:0] reg2_out = (e2_reg2_adr == 0) ? 0 : write_reg_en && (e2_reg2_adr == wb_rt_adr) ? write_val : e2_reg2_raw_val;

    wire[15:0] e2_result = (e2_op == 0) ? $signed(reg1_out) - $signed(reg2_out) : // sub
            (e2_op == 1) ? {{8{e2_imm[7]}}, e2_imm} : // movl
            (e2_op == 2) ? {e2_imm, reg2_out[7:0]} : // movh
            (e2_op >= 3 && e2_op <= 8) ? reg2_out: // jz, jnz, js, jns, ld, st
            wb_result; // Default case

    always @(posedge clk) begin
        wb_op <= e2_op;
        wb_rt_adr <= e2_rt_adr;
        wb_reg1_adr <= e2_reg1_adr;
        wb_reg2_adr <= e2_reg2_adr;
        wb_reg1_val <= reg1_out;
        wb_reg2_val <= reg2_out;
        wb_mem_forwarded <= mem_forwarded;
        wb_mem_forward_val <= mem_forward_val;
        wb_result <= e2_result;
        wb_branch <= (e2_op == 3) ? $signed(reg1_out) == 0 :
            (e2_op == 4) ? $signed(reg1_out) != 0 :
            (e2_op == 5) ? $signed(reg1_out) < 0 :
            (e2_op == 6) ? $signed(reg1_out) >= 0 :
            0;
        wb_halt <= (e2_op > 8) ? 1 : 0;
    end
// --------------- writeback
    reg[15:0] mem_read_buffer = 0; // should contain the lower word of the misaligned load
    reg[15:0] mem_write_buffer = 0; // should contain the 
    wire[15:0] mem_read_raw; //tood: branch 2 cycels earlier
    // if misaligned store, the first half of the st instruction has the address in its wb_reg1_val (these contain address of the store), and the second half of the st instruction has the address + 2 in its wb_reg1_val 
    wire[15:0] mem_read = wb_mem_forwarded ? wb_mem_forward_val : mem_read_raw;
    wire[15:0] write_val = (wb_op == 8 && wb_misaligned_st) ? {wb_result[7:0], mem_read[7:0]} : //first part of misaligned store
     (wb_op == 8 && wb2_misaligned_st) ? {mem_read[15:8], wb_result[15:8]} :
     (wb_op == 7) ? (wb_misaligned_ld ? {mem_read[7:0], mem_read_buffer[15:8]} : mem_read) :
      wb_result;
    wire write_reg_en = val_wb && (wb_op == 0 || wb_op == 1 || wb_op == 2 || wb_op == 7);
    wire write_mem_en = val_wb && wb_op == 8;
    assign branch = val_wb && wb_branch;
    wire self_modifying = (f2_pc == wb_reg1_val) || (d_pc == wb_reg1_val) || (e1_pc == wb_reg1_val) || (e2_pc == wb_reg1_val) ||(wb_pc == wb_reg1_val);
    assign flush = val_wb && ((write_mem_en && self_modifying));
    wire[15:0] jump_adr = branch ? wb_result : flush ? wb_pc + 2: pc + 2;
    wire jump_misaligned = val_wb && (flush || branch) && (jump_adr[0] == 1);
    always @(posedge clk) begin
        if (wb_rt_adr == 0 && write_reg_en && val_wb) begin
            $write("%c", write_val[7:0]);
        end
        pc <= flush ? wb_pc + 2 : branch ? wb_result : stall ? pc : pc + 2;
        mem_read_buffer <= mem_read;
        mem_write_buffer <= write_val; //the second half of the store
    end
    wire[14:0] pc_read_adr = pc[0] == 1 ? pc[15:1] + 1 : pc[15:1];
    // memory
    mem mem(clk,
         pc_read_adr,ins_raw, // fetch
         jump_misaligned?jump_adr[15:1]:mem_read_adr[15:1],mem_read_raw, // e1
         write_mem_en,wb_reg1_val[15:1],write_val); // wb
    // registers
    regs regs(clk,
        reg1,reg1_raw_out, // decode register reading
        reg2,reg2_raw_out, // decode register reading
        write_reg_en,wb_rt_adr,write_val); // writeback register writing
endmodule