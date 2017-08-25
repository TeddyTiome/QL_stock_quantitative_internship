#include "include/qlSim.h"
namespace
{
class t1: public AlphaBase
{
    public:
    explicit t1(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        Num1(cfg->getAttributeIntDefault("para1",5)),
	    Num2(cfg->getAttributeIntDefault("para2",20)),
        Num3(cfg->getAttributeIntDefault("para3",12)),
        Num4(cfg->getAttributeIntDefault("para4",10))

    {
    	MA5.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            MA5[di].resize(GLOBAL::Instruments.size(),nan);
        }

    	MA10.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            MA10[di].resize(GLOBAL::Instruments.size(),nan);
        }

        vol_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            vol_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }

    }
    void generate(int di) override
    {

			vector<float> rank1(GLOBAL::Instruments.size(),nan);
            vector<float> rank2(GLOBAL::Instruments.size(),nan);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

                vector<float> c1(Num1,nan);
                vector<float> c2(Num2,nan);

                for (int j = 0; j < Num1; ++ j)
                {
                    c1[j] = close[di - delay - j][ii];
                }                

                for (int i = 0; i < Num2; ++ i)
                {
                    c2[i] = close[di - delay - i][ii];
                }

                MA5[di-delay][ii] = QL_Oputils::mean(c1);
                MA10[di-delay][ii] = QL_Oputils::mean(c2);
            }
        }

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	vol_rank[di-delay][ii] = vol[di-delay][ii]; 
            }
        }

        QL_Oputils::rank(vol_rank[di-delay]);

       for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
				rank1[ii] = (MA5[di-delay][ii]-MA10[di-delay][ii]) -  2 * (MA5[di-delay-1][ii]-MA10[di-delay-1][ii]) + (MA5[di-delay-2][ii]-MA10[di-delay-2][ii]);
            }
        }

        QL_Oputils::rank(rank1);


        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                vector<float> c1(Num3,nan);
                vector<float> c2(Num3,nan);
                vector<float> c3(Num4,nan);
                for (int j = 0 ; j<Num4; ++j)
                {
		          for (int i =0; i<Num3; ++i)
                  {
                    c1[i] = high[di-delay-i-j][ii];
                    c2[i] = low[di-delay-i-j][ii];
                  }
                    c3[j] = QL_Oputils::corr(c1,c2);
                }

            QL_Oputils::rank(c3);
            rank2[ii] = c3[0];
        }
        }

        QL_Oputils::rank(rank2);



        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
                alpha[ii]=  -1 * powf(rank1[ii],rank2[ii]) ;                    
			}
	    }


        return;
		}
		
    
    
    
    void checkPointSave(boost::archive::binary_oarchive &ar) 
    {
        ar & *this;
    }
    void checkPointLoad(boost::archive::binary_iarchive &ar)
    {
        ar & *this;
    }
    std::string version() const
    {
        return GLOBALCONST::VERSION;
    }
    private:
        friend class boost::serialization::access;
        template<typename Archive>
        void serialize(Archive & ar, const unsigned int/*file_version*/)
        {
            ar & MA5;
            ar & MA10;
            ar & vol_rank;
        }
	const QL_MATRIX<QL_FLOAT> &vol;
    const QL_MATRIX<QL_FLOAT> &close;
    const QL_MATRIX<QL_FLOAT> &low;
    const QL_MATRIX<QL_FLOAT> &high;
    vector<vector<float> > MA5;
    vector<vector<float> > MA10;
    vector<vector<float> > vol_rank;

    int Num1; 
	int Num2;
    int Num3;
    int Num4;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new t1(cfg);
        return str;
    }
}

