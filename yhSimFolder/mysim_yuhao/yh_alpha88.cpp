#include "include/qlSim.h"
namespace
{
class alpha88: public AlphaBase
{
    public:
    explicit alpha88(XMLCONFIG::Element *cfg):
        AlphaBase(cfg), 
        vol(dr.getData<QL_MATRIX<QL_FLOAT>>("volume")),
        open(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_open")),
        close(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_close")),
        high(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_high")),
        low(dr.getData<QL_MATRIX<QL_FLOAT>>("adj_low")),
        Num1(cfg->getAttributeIntDefault("para1",8)),
	Num2(cfg->getAttributeIntDefault("para2",60)),
	Num3(cfg->getAttributeIntDefault("para3",20)),
	Num4(cfg->getAttributeIntDefault("para4",6)),
	Num5(cfg->getAttributeIntDefault("para5",2))

	{
        adv60.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            adv60[di].resize(GLOBAL::Instruments.size(),nan);
        }

        cor.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            cor[di].resize(GLOBAL::Instruments.size(),nan);
        }

        open_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            open_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }
        
        low_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            low_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }

        high_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            high_rank[di].resize(GLOBAL::Instruments.size(),nan);
        }

        close_rank.resize(GLOBAL::Dates.size());
        for (int di =0; di<GLOBAL::Dates.size(); ++di)
        {
            close_rank[di].resize(GLOBAL::Instruments.size(),nan);
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

                vector<float> c1(Num2,nan);

                for (int j = 0; j < Num2; ++ j)
                {
                    c1[j] = vol[di - delay - j][ii];
                }
                adv60[di-delay][ii] = QL_Oputils::mean(c1);
            }
        }


    	for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {
            	open_rank[di-delay][ii] = open[di-delay][ii];
            	low_rank[di-delay][ii] = low[di-delay][ii];
            	high_rank[di-delay][ii] = high[di-delay][ii];
            	close_rank[di-delay][ii] = close[di-delay][ii];
            }
        }

    	for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {	
            	vector<float> c1(Num1,nan);

            	rank1[ii] = 0.0;
            	for (int a= 0; a<Num1;++a){
            	    // QL_Oputils::rank(open_rank[di-delay-a]);
            	    // QL_Oputils::rank(low_rank[di-delay-a]);
            	    // QL_Oputils::rank(high_rank[di-delay-a]);
            	    // QL_Oputils::rank(close_rank[di-delay-a]);
				c1[a] = open_rank[di-delay-a][ii]+low_rank[di-delay-a][ii]-high_rank[di-delay-a][ii]-close_rank[di-delay-a][ii];
				rank1[ii] += float(c1[a]) *(Num1 -a)/((Num1+1)*Num1/2.0);
			}
            }
        }

        QL_Oputils::rank(rank1);
		
		for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {	               
            	vector<float> c1(Num1,nan);
				vector<float> c3(Num3,nan);
				vector<float> c4(Num1,nan);
				vector<float> c5(Num1,nan);

				for (int d=0; d<Num1;++d){
					for (int e=0;e<Num1;++e){
						c1[e]=close[di-delay-d-e][ii];
					}
					QL_Oputils::rank(c1);

					for (int f=0; f<Num3;++f){
						c3[f]=adv60[di-delay-f-d][ii];
					}
					QL_Oputils::rank(c3);

					c4[d]=c1[0];
					c5[d]=c3[0];
				}
				cor[di-delay][ii] = QL_Oputils::corr(c4,c5);
			}
		}

        for(int ii = 0; ii < GLOBAL::Instruments.size(); ++ ii)
        {
            if((valid[ di ][ ii ]))
            {

				vector<float> c6(Num4,nan);
				vector<float> c7(Num5,nan);

				float f2=0;
			
			for (int b=0; b<Num5;++b){
				f2 = 0.0;
				for (int c=0; c<Num4;++c){

					c6[c]= cor[di-delay-c-b][ii];

					f2 += float(c6[c]) *(Num4 -c)/((Num4+1)*Num4/2.0);
				}
				c7[b] = f2;
			}
			QL_Oputils::rank(c7);

			rank2[ii] = c7[0];


		if (float(rank1[ii])<float(rank2[ii]))
			{alpha[ii]=float(rank1[ii]);}
		else 
			{alpha[ii]=float(rank2[ii]);}
	
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
        }
	const QL_MATRIX<QL_FLOAT> &vol;
	const QL_MATRIX<QL_FLOAT> &open;
	const QL_MATRIX<QL_FLOAT> &close;
	const QL_MATRIX<QL_FLOAT> &high;
	const QL_MATRIX<QL_FLOAT> &low;
    vector<vector<float> > adv60;
    vector<vector<float> > cor;
    vector<vector<float> > open_rank;
    vector<vector<float> > low_rank;
    vector<vector<float> > high_rank;
    vector<vector<float> > close_rank;

        int Num1; 
	int Num2;
	int Num3;
	int Num4;
	int Num5;
};
}
extern "C"
{
    AlphaBase * createStrategy(XMLCONFIG::Element *cfg)
    {
        AlphaBase * str = new alpha88(cfg);
        return str;
    }
}

