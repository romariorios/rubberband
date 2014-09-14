class Grammar
  @@grammar = {
    :start          => [[:block_body]],
    :block_body     => [[:stm_list, :ret_stm], [:stm_list], [:ret_stm]],
    :stm_list       => [[:stm, :sep, :stm_list], [:stm]],
    :ret_stm        => [[:exclamation, :expr_value]],
    :stm            => [[:msg_send], [:expr_value]],
    :msg_send       => [[:expr_value, :stm]],
    :expr_value     => [[:literal], [:par_open, :stm, :par_close]],
    :literal        => [[:empty], [:symbol], [:number], [:array], [:table], [:block], [:dollar], [:tilde], [:at]],
    :empty          => [[:brack_open, :brack_close]],
    :array          => [[:bar, :array_body], [:bar, :array_count, :bar, :array_body]],
    :array_body     => [[:array_elements], [:brack_open, :array_elements, :brack_close], [:brack_open, :brack_close]],
    :array_count    => [[:number], [:msg_send]],
    :array_elements => [[:stm, :array_elements], [:stm]],
    :table          => [[:colon, :table_body]],
    :table_body     => [[:table_entries], [:brack_open, :table_entries, :brack_close], [:brack_open, :brack_close]],
    :table_entries  => [[:table_entry, :table_entries], [:table_entry]],
    :table_entry    => [[:table_index, :arrow, :stm]],
    :table_index    => [[:symbol], [:msg_send]],
    :block          => [[:curly_open, :block_body, :curly_close], [:curly_open, :curly_close]]
  }

  def self.terminal?(symbol)
    !@@grammar.has_key? symbol
  end

  def self.first(symbol)
    if Grammar.terminal? symbol
      symbol
    else
      @@grammar[symbol].map{ |r| Grammar.first(r[0]) }.flatten.uniq
    end
  end

  def self.follow(symbol, limit = 5)
    if Grammar.terminal? symbol or limit <= 0
      []
    else
      # Select the nonterminals which produce symbol
      @@grammar.select do |i, rules|
        rules.any?{ |r| r.any? { |item| item == symbol } }

      # Select the productions containing symbol
      end.each_with_object({}) do |(i, rules), res|
        res[i] = rules.select { |r| r.any? { |item| item == symbol } }

      # Select follow items
      end.map do |i, rules|
        rules.map do |r|
          r.each_with_index.map do |item, index|
            if i == :start
              :eof
            elsif index == r.size - 1
              Grammar.follow(i, limit - 1)
            else
              Grammar.first r[index + 1]
            end
          end
        end
      end.flatten.uniq
    end
  end
end

